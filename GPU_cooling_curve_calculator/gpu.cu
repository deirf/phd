#include <sys/time.h>

#include "device_launch_parameters.h"
#include "gpu.h"
#include "curve_helper.cu"
#include "io_helper.c"

#define WARP_SIZE 32
#define SHM_MAX_MAX (48 * 1024)
#define SHM_MAX_MAX_FOR_ALLOC (12 * 1024)



__constant__ GPUConfig d_config;

__shared__ int TCIndex;
__shared__ int lastTemperatureIndex;
__shared__ PREC dtpdx2;
__shared__ PREC dtdx2m2;
__shared__ int htc_len;
__shared__ int cooling_curve_len;
__shared__ PREC temperature_final;
__shared__ int temperatures_len;
__shared__ PREC temperature_init;
__shared__ PREC end_time;
__shared__ PREC delta_t;
__shared__ int interpolated_htc_by_dt_len;
__shared__ int calculation_count;



inline int
warponizing(int _num)
{
  return (int)ceil( _num / (PREC)WARP_SIZE ) * WARP_SIZE;
}

#define CHECK_CUDA_ERROR(val) check((val), #val, __FILE__, __LINE__)
template <typename T>
void check(T err, char const* const func, char const* const file,
           int const line)
{
    if (err != cudaSuccess)
    {
        std::cerr << "CUDA Runtime Error at: " << file << ":" << line
                  << std::endl;
        std::cerr << cudaGetErrorString(err) << " " << func << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

#define CHECK_LAST_CUDA_ERROR() checkLast(__FILE__, __LINE__)
void checkLast(char const* const file, int const line)
{
    cudaError_t err{cudaGetLastError()};
    if (err != cudaSuccess)
    {
        std::cerr << "CUDA Runtime Error at: " << file << ":" << line
                  << std::endl;
        std::cerr << cudaGetErrorString(err) << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

__device__ void
FillArrayAll(PREC* const _array, const PREC _init_value, const int _len)
{
  const int cicle = _len / blockDim.x;
  const int filled_items = cicle * blockDim.x;

  for ( int i = 0; i < cicle; i++ )
  {
    const int array_index = threadIdx.x + i * blockDim.x;
    _array[ array_index ] = _init_value;
  }

  if ( threadIdx.x < _len - filled_items )
  {
    const int array_index = filled_items + threadIdx.x;
    _array[ array_index ] = _init_value;
  }

  __syncthreads();
}

__device__ void
CopyArrayAll(PREC* const _dst, const PREC* const _src, const int _len)
{
  const int cicle = _len / blockDim.x;
  const int copied_items = cicle * blockDim.x;

  for ( int i = 0; i < cicle; i++ )
  {
    const int array_index = threadIdx.x + i * blockDim.x;
    _dst[ array_index ] = _src[ array_index ];
  }

  if ( threadIdx.x < _len - copied_items )
  {
    const int array_index = copied_items + threadIdx.x;
    _dst[ array_index ] = _src[ array_index ];
  }

  __syncthreads();
}

__device__ void
CalculateCooling_curve(
                       PREC* _temperatures,
                       PREC* _nextTemperatures,
                       PREC _decTmodmHTC,
                       PREC* _c1pc1p2i,
                       PREC* _c1mc1p2i
                      )
{
  if ( threadIdx.x > WARP_SIZE && threadIdx.x < lastTemperatureIndex + WARP_SIZE )
  {
    const int idx = threadIdx.x - WARP_SIZE;
    const PREC temp_i = _temperatures[ idx ];
    const PREC temp_im1 = _temperatures[ idx - 1 ];
    const PREC temp_ip1 = _temperatures[ idx + 1 ];
    const PREC alpha = CalculateAlpha( temp_i );
    const PREC c1pc1p2i = _c1pc1p2i[ idx ];
    const PREC c1mc1p2i = _c1mc1p2i[ idx ];

    _nextTemperatures[ idx ] = temp_i + (alpha * dtpdx2) * ( temp_ip1 * c1pc1p2i + temp_im1 * c1mc1p2i - 2 * temp_i );
  }

  if ( threadIdx.x == 0 )
  {
    const PREC temp_0 = _temperatures[ 0 ];
    const PREC temp_1 = _temperatures[ 1 ];
    const PREC alpha = CalculateAlpha( temp_0 );

    _nextTemperatures[ 0 ] = temp_0 + (alpha * dtdx2m2) * ( temp_1 - temp_0 );
  }

  if ( threadIdx.x == lastTemperatureIndex + WARP_SIZE )
  {
    const PREC temp_last = _temperatures[ lastTemperatureIndex ];
    const PREC temp_lastm1 = _temperatures[ lastTemperatureIndex - 1 ];
    const PREC alpha = CalculateAlpha( temp_last );
    const PREC c1pK_last_m2 = 1 / (2 * CalculateK( temp_last ));
    const PREC decrement_T  = ( temp_last - temperature_final ) * _decTmodmHTC * c1pK_last_m2;

    _nextTemperatures[ lastTemperatureIndex ] = temp_last + (alpha * dtdx2m2) * ( temp_lastm1 - temp_last - decrement_T );
  }
}

__device__ void
StoreData(
                PREC*  const _cooling_curve_values,
                const PREC _value,
                int* const _store_index
               )
{
  _cooling_curve_values[ *_store_index ] = _value;
  (*_store_index)++;
}

__device__ void
interpolated_htc_by_dt(
             PREC* const _interpolated_dt_htc_value,
       const PREC* const _htc_times,
       const PREC* const _htc_values,
             int* const _start
       )
{
  const int len = min( interpolated_htc_by_dt_len, (calculation_count - *_start));
  const int cicle = len / blockDim.x;
  const int calculated_items = cicle * blockDim.x;

  for ( int i = 0; i < cicle; i++ )
  {
    const int index = threadIdx.x + i * blockDim.x;
    const PREC time = delta_t * (index + *_start);
    _interpolated_dt_htc_value[ index ] = LinearInterpolationWithFixDx( _htc_times, _htc_values, htc_len, time );
  }


  if ( threadIdx.x < len - calculated_items )
  {
    const int index = calculated_items + threadIdx.x;
    const PREC time = delta_t * (index + *_start);
    _interpolated_dt_htc_value[ index ] = LinearInterpolationWithFixDx( _htc_times, _htc_values, htc_len, time );
  }

  *_start += len;
  __syncthreads();
}

__global__ void
CalculateCooling_curveWorker(
                            const int _number_of_curve,
                            const int _interpolated_htc_by_dt_len,
                            const int _calculation_count,
                            const PREC  _decTmod,
                            const PREC  _dtpdx2,
                            const PREC  _dtdx2m2,
                            const PREC  _delta_t,
                                  PREC* const _g_htc_values,
                            const PREC* const _g_htc_times,
                                  PREC* const _g_cooling_curve_values,
                            const PREC* const _g_cooling_curve_times,
                            const PREC* const _g_c1pc1p2i,
                            const PREC* const _g_c1mc1p2i
                           )
{
  if ( blockIdx.x < _number_of_curve )
  {
    extern __shared__ __volatile__ PREC dyn_share_memory[];

    if (threadIdx.x == 0)
    {
      end_time = d_config.end_time;
      htc_len = d_config.htc_N;
      cooling_curve_len = d_config.cooling_curve_N;
      temperatures_len = d_config.N;
      temperature_init = d_config.initial_temperature;
      temperature_final = d_config.final_temperature;
      lastTemperatureIndex = d_config.lastTemperatureIndex;
      TCIndex = d_config.TCIndex;
      delta_t = _delta_t;
      interpolated_htc_by_dt_len = _interpolated_htc_by_dt_len;
      calculation_count = _calculation_count;
      dtpdx2 = _dtpdx2;
      dtdx2m2 = _dtdx2m2;
    }
    __syncthreads();

    PREC* htc_values = (PREC*)&dyn_share_memory;
    PREC* htc_times = htc_values + htc_len;
    PREC* cooling_curve_values = htc_times + htc_len;
    PREC* cooling_curve_times = cooling_curve_values + cooling_curve_len;
    PREC* temperatures = cooling_curve_times + cooling_curve_len;
    PREC* nextTemperatures = temperatures + temperatures_len;
    PREC* c1pc1p2i = nextTemperatures + temperatures_len;
    PREC* c1mc1p2i = c1pc1p2i + temperatures_len;
    PREC* interpolated_htc_by_dt_value = c1mc1p2i + temperatures_len;

    PREC time = 0;
    PREC reference_time = 0;
    int ref_time_index = 0;
    int store_index = 0;
    int time_index = 0;
    int interpolated_htc_by_dt_next_start = 0;
    int interpolated_htc_by_dt_index = 0;

    PREC* htc_values_source = _g_htc_values + blockIdx.x * htc_len;
    PREC* cooling_curve_values_source = _g_cooling_curve_values + blockIdx.x * cooling_curve_len;

    CopyArrayAll(htc_values, htc_values_source, htc_len);
    CopyArrayAll(htc_times, _g_htc_times, htc_len);
    CopyArrayAll(cooling_curve_times, _g_cooling_curve_times, cooling_curve_len);
    CopyArrayAll(c1pc1p2i, _g_c1pc1p2i, temperatures_len);
    CopyArrayAll(c1mc1p2i, _g_c1mc1p2i, temperatures_len);

    FillArrayAll(temperatures, temperature_init, temperatures_len);

    interpolated_htc_by_dt(interpolated_htc_by_dt_value, htc_times, htc_values, &interpolated_htc_by_dt_next_start);

    StoreData(
               cooling_curve_values,
               temperatures[ TCIndex ],
               &store_index
              );

    ++time_index;
    ++ref_time_index;
    ++interpolated_htc_by_dt_index;
    time = delta_t;
    reference_time = cooling_curve_times[ ref_time_index ];

    while ( time < end_time )
    {
      const PREC htc = interpolated_htc_by_dt_value[ interpolated_htc_by_dt_index ];

      CalculateCooling_curve(
                            temperatures,
                            nextTemperatures,
                            _decTmod * htc,
                            c1pc1p2i, c1mc1p2i
                           );

      if ( time > reference_time )
      {
        const PREC prev_time = time - delta_t;
        const PREC prev_temperature = temperatures[ TCIndex ];
        const PREC temperature = nextTemperatures[ TCIndex ];
        const PREC reftime_temperature =
                   LinearInterpolateCalc(prev_time, time, prev_temperature, temperature, reference_time);
        StoreData(cooling_curve_values, reftime_temperature, &store_index);

        ++ref_time_index;
        reference_time = ref_time_index < cooling_curve_len ? cooling_curve_times[ ref_time_index ]: end_time;
      }
      SWAP( temperatures, nextTemperatures );

      ++interpolated_htc_by_dt_index;
      if ( interpolated_htc_by_dt_index == interpolated_htc_by_dt_len )
      {
        __syncthreads();
        interpolated_htc_by_dt_index = 0;
        interpolated_htc_by_dt(interpolated_htc_by_dt_value, htc_times, htc_values, &interpolated_htc_by_dt_next_start);
      }

      ++time_index;
      time = delta_t * time_index;
    }

    const PREC htc = interpolated_htc_by_dt_value[ interpolated_htc_by_dt_index ];

    CalculateCooling_curve(
                        temperatures,
                        nextTemperatures,
                        _decTmod * htc,
                        c1pc1p2i, c1mc1p2i
                       );

    StoreData(
               cooling_curve_values,
               nextTemperatures[ TCIndex ],
               &store_index
              );

    __syncthreads();
    CopyArrayAll(cooling_curve_values_source, cooling_curve_values, cooling_curve_len);
  }
}

static void
CalculateWorker(const int _number_of_curve, Config* config)
{
  struct timeval tval_before, tval_after;
  gettimeofday(&tval_before, NULL);

  PREC* d_htc_values;
  PREC* d_htc_times;
  PREC* d_cooling_curve_values;
  PREC* d_cooling_curve_times;

  PREC* d_c1pc1p2i;
  PREC* d_c1mc1p2i;

  GPUConfig gpuconfig;

  const int lastTemperatureIndex = config->N - 1;

  gpuconfig.htc_N = config->htc_N;
  gpuconfig.end_time = config->end_time;
  gpuconfig.initial_temperature = config->initial_temperature;
  gpuconfig.final_temperature = config->final_temperature;
  gpuconfig.R = config->R;
  gpuconfig.N = config->N;
  gpuconfig.TC = config->TC;
  gpuconfig.cooling_curve_N = config->cooling_curve_N;
  gpuconfig.lastTemperatureIndex = lastTemperatureIndex;
  gpuconfig.TCIndex = config->TC - 1;

  const PREC dx = config->R / (PREC)config->N;
  const PREC dx2 = dx * dx;
  const PREC c1pdx2 = 1.0f / dx2;
  const PREC c2mdx2 = 2.0f * dx2;
  const PREC dx4 = dx2 * dx2;
  const PREC delta_t = dx2 / 2.0f / H_CalculateAlpha(config->initial_temperature); //stability_dt
  const PREC decTmod = ( 2.0f + 1.0f / ( (PREC)lastTemperatureIndex * dx4 ) ) * dx;
  const PREC dtpdx2 = delta_t * c1pdx2;
  const PREC dtdx2m2 = delta_t * c2mdx2;
  const PREC calculation_count = (int)floor( config->end_time / delta_t + 1);

  config->c1pc1p2i[ 0 ] = 1;
  config->c1mc1p2i[ 0 ] = 1;
  for (int i = 1; i < config->N; ++i) // divide by zero if i=0
  {
    const PREC c1p2i = 1.0f / ( (PREC)(i << 1) );
    config->c1pc1p2i[ i ] = 1 + c1p2i;
    config->c1mc1p2i[ i ] = 1 - c1p2i;
  }

  cudaEvent_t start, stop;
  cudaEventCreate(&start);
  cudaEventCreate(&stop);

  CHECK_CUDA_ERROR(cudaMemcpyToSymbol((const void*)&d_config, &gpuconfig, sizeof(GPUConfig)));

  CHECK_CUDA_ERROR(cudaMalloc((void**)&d_cooling_curve_times, config->cooling_curve_N * sizeof(PREC)));
  CHECK_CUDA_ERROR(cudaMemcpy((void*)d_cooling_curve_times, (const void *)config->cooling_curve_times, config->cooling_curve_N * sizeof(PREC), cudaMemcpyHostToDevice));

  CHECK_CUDA_ERROR(cudaMalloc((void**)&d_htc_values, _number_of_curve * config->htc_N * sizeof(PREC)));
  CHECK_CUDA_ERROR(cudaMemcpy((void*)d_htc_values, (const void *)config->htc_values, _number_of_curve * config->htc_N * sizeof(PREC), cudaMemcpyHostToDevice));

  CHECK_CUDA_ERROR(cudaMalloc((void**)&d_htc_times, config->htc_N * sizeof(PREC)));
  CHECK_CUDA_ERROR(cudaMemcpy((void*)d_htc_times, (const void *)config->htc_times, config->htc_N * sizeof(PREC), cudaMemcpyHostToDevice));

  CHECK_CUDA_ERROR(cudaMalloc((void**)&d_c1pc1p2i, config->N * sizeof(PREC)));
  CHECK_CUDA_ERROR(cudaMemcpy((void*)d_c1pc1p2i, (const void *)config->c1pc1p2i, config->N * sizeof(PREC), cudaMemcpyHostToDevice));

  CHECK_CUDA_ERROR(cudaMalloc((void**)&d_c1mc1p2i, config->N * sizeof(PREC)));
  CHECK_CUDA_ERROR(cudaMemcpy((void*)d_c1mc1p2i, (const void *)config->c1mc1p2i, config->N * sizeof(PREC), cudaMemcpyHostToDevice));

  CHECK_CUDA_ERROR(cudaMalloc((void**)&d_cooling_curve_values, _number_of_curve * config->cooling_curve_N * sizeof(PREC)));

  dim3 threadNum = dim3( warponizing( config->N + 64), 1 );                        //grid_width
  dim3 blockNum =  dim3( _number_of_curve, 1);                                     //number of curve

  const int shared_mem_fixed_data_count =
                       (
                        config->htc_N // htc_values
                        +
                        config->htc_N // htc_times
                        +
                        config->cooling_curve_N //cooling_curve_values
                        +
                        config->cooling_curve_N //cooling_curve_times
                        +
                        config->N // temperatures
                        +
                        config->N // next_temperatures
                        +
                        config->N // c1pc1p2i
                        +
                        config->N // c1mc1p2i
                        // +
                        // config->cooling_curve_N // interpolated_htc_by_reftime
                       );

  const int interpolated_htc_by_dt_len = SHM_MAX_MAX_FOR_ALLOC / sizeof(PREC) - shared_mem_fixed_data_count;
  assume( interpolated_htc_by_dt_len > 0, "interpolated_htc_by_dt_len:%d is not pozitive", interpolated_htc_by_dt_len);

  int shared_mem_len = SHM_MAX_MAX_FOR_ALLOC;

  cudaEventRecord(start);

  CalculateCooling_curveWorker <<< blockNum, threadNum, shared_mem_len >>> (
                                                                            _number_of_curve,
                                                                            interpolated_htc_by_dt_len,
                                                                            calculation_count,
                                                                            decTmod,
                                                                            dtpdx2,
                                                                            dtdx2m2,
                                                                            delta_t,
                                                                            d_htc_values,
                                                                            d_htc_times,
                                                                            d_cooling_curve_values,
                                                                            d_cooling_curve_times,
                                                                            d_c1pc1p2i,
                                                                            d_c1mc1p2i
                                                                           );

  CHECK_LAST_CUDA_ERROR();

  cudaEventRecord(stop);
  cudaEventSynchronize(stop);

  CHECK_CUDA_ERROR(cudaDeviceSynchronize());

  CHECK_CUDA_ERROR(cudaMemcpy((void*)config->cooling_curve_values, (const void *)d_cooling_curve_values, _number_of_curve * config->cooling_curve_N * sizeof(PREC), cudaMemcpyDeviceToHost));


  CHECK_CUDA_ERROR(cudaFree(d_cooling_curve_values));
  CHECK_CUDA_ERROR(cudaFree(d_cooling_curve_times));
  CHECK_CUDA_ERROR(cudaFree(d_htc_values));
  CHECK_CUDA_ERROR(cudaFree(d_htc_times));

  float gpu_ms;
  gettimeofday(&tval_after, NULL);
  cudaEventElapsedTime(&gpu_ms, start, stop);
  printf("%f msec\n", gpu_ms);
}

static void
calculate(int _number_of_curve, int _N, bool _write)
{
  Config config;

  config.R = 0.020;
  config.N = _N;
  config.TC = _N;
  config.initial_temperature = 850.0;
  config.final_temperature = 20.0;
  config.cooling_curve_N = 600;
  config.htc_N = 200;

  cudaMallocHost( (void**)&config.htc_times, config.htc_N * sizeof(PREC) );
  cudaMallocHost( (void**)&config.c1pc1p2i, config.N * sizeof(PREC) );
  cudaMallocHost( (void**)&config.c1mc1p2i, config.N * sizeof(PREC) );
  cudaMallocHost( (void**)&config.htc_values, _number_of_curve * config.htc_N * sizeof(PREC) );
  cudaMallocHost( (void**)&config.cooling_curve_times, config.cooling_curve_N * sizeof(PREC) );
  cudaMallocHost( (void**)&config.cooling_curve_values, _number_of_curve * config.cooling_curve_N * sizeof(PREC) );

  read_htc(&config,"./test_data/htc_time.dat","./test_data/htc_value.dat");
  reproduce_cooling_curve(&config);
  reproduce_htc_values(&config, _number_of_curve);

  CalculateWorker( _number_of_curve, &config);

  if (_write) write_result(&config, _number_of_curve);

  cudaFreeHost(config.cooling_curve_values);
  cudaFreeHost(config.htc_values);
  cudaFreeHost(config.htc_times);
  cudaFreeHost(config.cooling_curve_times);
  cudaFreeHost(config.c1pc1p2i);
  cudaFreeHost(config.c1mc1p2i);
}

int
main(int argc, char **argv)
{
  calculate(1, 32, false);
  calculate(32, 96, false);
  calculate(1000, 96, true);
  calculate(10000, 96, true);
}

// nvcc -arch=sm_75 -O3 ./gpu.cu -o gpu
// nvcc -arch=sm_75 -O3 -v ./gpu.cu -o gpu
// nvcc -arch=sm_75 -O3 --resource-usage ./gpu.cu -o gpu
// nvcc -arch=sm_75 -O3 --maxrregcount=32 --resource-usage ./gpu.cu -o gpu
// nvcc -O0 -lineinfo ./gpu.cu --maxrregcount=64  -o gpu
