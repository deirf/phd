#pragma once

#include <string>
#include <cfloat>
#include <algorithm>
#include <thread>
#include <cmath>
#include <time.h>
#include <map>
#include <vector>
#include <cstring>
#include <signal.h>
#include <typeinfo>
#include <typeindex>
#include <unordered_map>
#include <sys/mman.h>
#include <malloc.h>
#include <unistd.h>

#ifndef __NVCC__
  #ifdef __AVX__
    #define __AVX_CODE__ 1
    #define __CPU_CODE__ 1
  #else
    #define __STD_CODE__ 1
    #define __CPU_CODE__ 1
  #endif
#else
  #define __GPU_CODE__ 1
#endif

#if defined(__GNUC__) || defined(_MSC_VER) || defined(__restrict)
#define restrict __restrict
#elif !defined(restrict) // restrict or __restrict not supported in C++
#define restrict
#endif

#define GPU_1D_BLOCKSIZE 256


#define EPSILON 1e-38f
// #define PREC float
#define PREC double

#define COUNT_T uint64_t
#define COUNT_T_MAX (UINT64_MAX - 1)
#define COUNT_T_MIN 0
#define COUNT_T_NEG UINT64_MAX
#define CHECK_COUNT_T_NOT_NEG(x) (x==COUNT_T_NEG)
#define CHECK_COUNT_T_NEG(x) (x!=COUNT_T_NEG)

// #define PREC_MIN FLT_MIN
// #define PREC_MAX FLT_MAX
#define PREC_MIN DBL_MIN
#define PREC_MAX DBL_MAX

// #define PROC_PREC float
#define PROC_PREC double
#define PROC_COUNT_T uint64_t
#define PREC_REF_FORMAT "%.20f"

#define TRUE true
#define FALSE false
#define PI 3.141592653589793238463f
#define RAD ( PI / 180.0f )
#define SPARK_UNNAMED_HISTORY_SIZE 5
#define BEST_SPARK_INDEX 0
#define FIREWORK_SPARK_INDEX 0
#define EPSILON_ALMOST_ZERO_LIMIT 1e-16f
#define INT_FORMAT "%6.0f"
#define PREC_FORMAT "%13.4f"
#define DOUBLE_FORMAT "%34.17f"
#define PREC_PERCENT_FORMAT "%8.2f"
#define LOCATION_PRINT_FORMAT DOUBLE_FORMAT

#define MAX_EQ_TERRAIN_COUNTER 3
#define SPARKLIST_SPARKCOUNT 1
#define SPARKLIST_SPARKINDEX 0

#define FMAX_IF_FMAX_LESS_THAN_1 1000
#define FIREWORK_INIT_AMAX 1

#define LIKELY(x)   (__builtin_expect((x),1))
#define UNLIKELY(x) (__builtin_expect((x),0))

#define SIGN(val) (int)(((val) > 0) - ((val) < 0))
#define MIN(a,b) ((PREC)(b < a) ? b : a)
#define MAX(a,b) ((PREC)(b < a) ? a : b)

#define UNUSED(expr) do { (void)(expr); } while (0)

#define TOPLIMIT(value, max) (UNLIKELY(value > max) ? max : value)
#define BOTTOMLIMIT(value, min) (UNLIKELY(value < min) ? min : value)

#define CONCAT(a, b) CONCAT_(a, b)
#define CONCAT_(a, b) a ## b

#define FILL(p, len, val) (std::fill_n(p, len, val))
#define SWAP(p1, p2) (std::swap(p1, p2))
#define ABS(val) (std::fabs(val))

#define PRINT_RESULT_NAME_LEN 20
#define PRINT_RESULT_ROUND_LEN 5
#define PRINT_RESULT_CALC_LEN 20
#define PRINT_RESULT_ITER_LEN 20
#define PRINT_RESULT_MEANF_LEN 15
#define PRINT_RESULT_MEDIANF_LEN 15
#define PRINT_RESULT_SHAPEFITNESS_LEN 15
#define PRINT_RESULT_FITNESS_LEN 30
#define PRINT_RESULT_STDDEV_LEN 15
#define PRINT_RESULT_STDERROR_LEN 15
#define PRINT_RESULT_RUNTIME_LEN 20
#define PRINT_RESULT_SPACE_LEN 2
#define PRINT_RESULT_MSG_LEN 15
#define PRINT_RESULT_MEANR_LEN 15
#define PRINT_RESULT_MEDIANR_LEN 15

#define PRINT_RESULT_NAME ""
#define PRINT_RESULT_ROUND "Round"
#define PRINT_RESULT_CALC "Calculations"
#define PRINT_RESULT_ITER "Iterations"
#define PRINT_RESULT_MEANF "Mean F."
#define PRINT_RESULT_MEDIANF "Median F."
#define PRINT_RESULT_FITNESS "Fitness"
#define PRINT_RESULT_SHAPEFITNESS "ShapeFitness"
#define PRINT_RESULT_STDDEV "StdDeviation"
#define PRINT_RESULT_STDERROR "StdError"
#define PRINT_RESULT_RUNTIME "RunTime (sec)"
#define PRINT_RESULT_MSG "Message"
#define PRINT_RESULT_MEANR "Mean R."
#define PRINT_RESULT_MEDIANR "Median R."

#define PRINT_STAT_POPULATION_POS PRINT_RESULT_NAME_LEN +  \
                                  PRINT_RESULT_SPACE_LEN + \
                                  PRINT_RESULT_ROUND_LEN + \
                                  PRINT_RESULT_SPACE_LEN
#define PRINT_STAT_ITER_POS 100
#define PRINT_STAT_LOG 150

#define NFWA_LOGWORKER_INTS_COUNT 2
#define MAX_BEST_SPARK_NUMBER 1
#define MAX_FIREWORK_SPARK_NUMBER 1

#define SWARM_LOGWORKER_PREC_ARRAY_COUNT 4
#define SWARM_LOGWORKER_COUNT_ARRAY_COUNT 0
#define SWARM_LOGWORKER_LARRAY_COUNT 0
#define SWARM_LOGWORKER_INTS_COUNT 0

#define OFWA_LOGWORKER_PREC_ARRAY_COUNT 3
#define OFWA_LOGWORKER_COUNT_ARRAY_COUNT 0
#define OFWA_LOGWORKER_LARRAY_COUNT 0
#define OFWA_LOGWORKER_INTS_COUNT 0

#define LOGGING_PROGRESS_COUNTER_MAX 1

inline long to_usec(const struct timespec& ts) { return (long int)((double)ts.tv_sec * 1e6 + (double)ts.tv_nsec * 1e-3); }

inline long
_clock_start()
{
  struct timespec ts;
  while ( clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts) == -1 ) { };
  return to_usec(ts);
}

inline void
_clock_stop(long clock_start)
{
  struct timespec ts;
  while ( clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts) == -1 ) { };
  const long clock_finish = to_usec(ts);
  const long elapsed_ts = clock_finish - clock_start;
  printf("%lfmsec\n", (PREC)elapsed_ts/(PREC)1000 );
}

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(*array))

const uint THREADPOOL_SIZE = (uint)std::max(1, (int) std::thread::hardware_concurrency() - 1);

extern sig_atomic_t simutation_stop_running;

#define _malloc(len) aligned_malloc(len)
#define _malloc0(len) aligned_malloc0(len)
#define _free(len) aligned_free(len)

// #define _malloc(len) malloc(len)
// #define _malloc0(len) calloc(len)
// #define _free(len) free(len)

#define DOES_NOT_REACH() assume(FALSE, "Wrong code path")

#define assume(_condition, ...)                                                   \
do {                                                                              \
  if ( UNLIKELY( !( _condition ) ) )                                              \
  {                                                                               \
    fprintf(stderr, "\n\n  ");                                                    \
    fprintf(stderr, ##__VA_ARGS__);                                               \
    fprintf(stderr, "\n  Assume checked is placed at (%s:%d in function:'%s')\n", \
                    __FILE__, __LINE__, __FUNCTION__);                            \
    raise(SIGABRT);                                                               \
  }                                                                               \
} while (0)

inline bool
is_aligned(const void* restrict ptr, std::uintptr_t alignment) noexcept {
    std::uintptr_t iptr = reinterpret_cast<std::uintptr_t>(ptr);
    return !(iptr % alignment);
}

inline void*
aligned_malloc(const COUNT_T size)
{
  if (!size)
  {
    return NULL;
  }

  void* result = NULL;
  const COUNT_T alignment = (COUNT_T)sysconf(_SC_LEVEL1_DCACHE_LINESIZE );

  assume( alignment >= sizeof(uint64_t), "Alignemnt size is less than '%ld'", sizeof(uint64_t) );
  assume( ( alignment % sizeof(uint64_t) ) == 0, "Alignemnt does not fit for uint64" );

  int error = posix_memalign((void **)&result, (size_t)alignment, (size_t)size);

  assume( error == 0, "Posix memalign has an error='%d'", error );
  assume( result != NULL, "Posix memalign has no result" );
  assume( is_aligned(result, alignment), "Allocated memory is not aligned" );

  const COUNT_T alloc_size = malloc_usable_size(result);
  if ( mlock(result, alloc_size) )
  {
    free(result);
    assume(FALSE, "Unsuccessful HEAP memory allocation; size=%ld", size);
  }

  return result;
}

inline void*
aligned_malloc0(const COUNT_T size)
{
  void* result = aligned_malloc(size);
  assume( result, "Aligned malloc returned with NULL" );

  std::memset(result, 0, size);

  return result;
}

inline void
aligned_free(void* restrict ptr)
{
  if (!ptr)
  {
    return;
  }

  COUNT_T size = malloc_usable_size(ptr);
  munlock(ptr, size);
  free(ptr);
}

template <typename fromType, typename toType>
inline bool
representable_as_type(fromType _value)
{
    const toType repr = (toType) _value;
    return _value >= 0L && (fromType)repr == _value;
}

#ifdef __GPU_CODE__
  #pragma nv_diag_suppress 191  // warning #191-D: type qualifier is meaningless on cast type
#endif
inline __attribute__((nonnull)) void
memcopy_small(uint8_t* restrict dst, const uint8_t* restrict src, size_t size)
{
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wcast-align" // increases required alignment of target type
  if (size == 8)
  {
      *(uint64_t* restrict)dst = *(const uint64_t* restrict)src;
      return;
  }

  if (size >= 4)
  {
      *(uint32_t* restrict)dst = *(const uint32_t* restrict)src;
      dst += 4;
      src += 4;
  }

  if (size & 2)
  {
      *(uint16_t* restrict)dst = *(const uint16_t* restrict)src;
      dst += 2;
      src += 2;
  }
  #pragma GCC diagnostic pop

  if (size & 1)
  {
      *dst = *src;
  }
}

inline __attribute__((nonnull)) void*
_memcpy_fast(void* restrict dst, const void* restrict src, COUNT_T size)
{
    uint8_t* dst8 = (uint8_t*)dst;
    const uint8_t* src8 = (const uint8_t*)src;

    if (size > 8)
    {
        const COUNT_T qwords = size >> 3;

        uint64_t* restrict dst64 = (uint64_t*)dst;
        const uint64_t* restrict src64 = (uint64_t*)src;

        for (COUNT_T i = 0; i < qwords; i++)
        {
            *dst64++ = *src64++;
        }

        const COUNT_T aligned_size = qwords << 3;

        size -= aligned_size;
        dst8 += aligned_size;
        src8 += aligned_size;
    }

    memcopy_small(dst8, src8, size);

    return dst;
}

template <typename dstType, typename srcType>
inline __attribute__((nonnull)) dstType*
_memcpy_convert(dstType* restrict dst, const srcType* restrict src, COUNT_T size)
{
    for (COUNT_T i = 0; i < size;i++)
    {
        dst[i] = (dstType)src[i];
    }

    return dst;
}

template <typename type>
inline __attribute__((nonnull)) type*
_memcpy_typed(type* restrict dst, const type* restrict src, COUNT_T size)
{
    for (COUNT_T i = 0; i < size;i++)
    {
        dst[i] = src[i];
    }

    return dst;
}

template <typename RecordType>
  class Record;

template <typename ArrayType>
  class Array;

template <typename ArrayType>
  class LArray;

class Spark;
class RandomSpark;
class GaussianSpark;
class QuantumSpark;
class FireworkSpark;
class BestSpark;
class Firework;

#define TVector(type) std::vector< type >
#define TRecord(type) Record< type >
#define TArray(type) Array< type >
#define TLArray(type) LArray< type >
#define TVRecord(type) TVector(TRecord(type)*)
#define TVArray(type) TVector(TRecord(type)*)
#define TVLArray(type) TVector(TArray(type)*)
#define TFirework TRecord(Firework*)
#define TRandomSpark TRecord(RandomSpark*)
#define TGaussianSpark TRecord(GaussianSpark*)
#define TFireworkSpark TRecord(FireworkSpark*)
#define TBestSpark TRecord(BestSpark*)
#define TQuantumSpark TRecord(QuantumSpark*)
#define THistoryData TRecord(SparkRecordData*)
#define TSparkListMap std::map< COUNT_T, COUNT_T >


typedef struct _sparkrecorddata
{
  TRecord(PREC)* param = NULL;
  TRecord(PREC)* calculatedFunctionValue = NULL;
  TRecord(PREC)* fitnessFunction = NULL;
  TRecord(PREC)* property = NULL;
  TRecord(PREC)* statistic = NULL;
  TRecord(COUNT_T)* state = NULL;
  TRecord(PREC)* custom = NULL;
  TRecord(PREC)* realAmaxFunction = NULL;
  TArray(PREC)* customLocation = NULL;
  TRecord(Spark*)* spark = NULL;
} SparkRecordData;

typedef struct _sparkarraydata
{
  TArray(PREC)* param = NULL;
  TArray(PREC)* calculatedFunctionValue = NULL;
  TArray(PREC)* fitnessFunction = NULL;
  TArray(PREC)* property = NULL;
  TArray(PREC)* statistic = NULL;
  TArray(COUNT_T)* state = NULL;
  TArray(PREC)* custom = NULL;
  TArray(PREC)* realAmaxFunction = NULL;
  TLArray(PREC)* customLocation = NULL;
  TArray(Spark*)* spark = NULL;
} SparkArrayData;

typedef enum
{
  SH_BEST = 0,
  SH_WORST,
  SH_HBEST,
  SH_NAMED_MAX,
  SH_FIRST = SH_NAMED_MAX,
  SH_ACTUAL = SH_FIRST,
  SH_PREV = SH_FIRST + 1,
  SH_LAST = SH_FIRST + SPARK_UNNAMED_HISTORY_SIZE - 1,
  SH_UNNAMED_MAX = SH_LAST - SH_FIRST + 1,
  SH_MAX = SH_UNNAMED_MAX + SH_NAMED_MAX,
} ARRAY_SparkHistoryIndex;

typedef enum
{
  SDT_UNKNOWN = 0,
  SDT_NONE,
  SDT_TYPENAME,
  SDT_STATENAME,
  SDT_SUBSTATENAME,
  SDT_INT,
  SDT_PREC,
  SDT_DOUBLE,
  SDT_NO_PRINT,
  SDT_PERCENT,
  SDT_DIRECTION,
  SDT_TERRAIN,
  SDT_TERRAIN_DIR,
  SDT_TELEMETRIC_DIR,
  SDT_DOMSPEED_DIR,
  SDT_RECORD,
  SDT_ELEMENTTYPENAME,
  SDT_MAX,
} SparkDataType;

typedef enum
{
  SPT_HMIN = 0,
  SPT_HMAX,
  SPT_FMIN,
  SPT_FMAX,
  SPT_FITNESS,
  SPT_SHAPE_FITNESS,
  SPT_NORM_FITNESS,
  SPT_ITL,
  SPT_EFFICIENCY,
  SPT_MAX,
} ARRAY_SparkPropertyTypeIndex;

const std::string SparkPropertyTypeName[] =
      { "HMIN", "HMAX", "FMIN", "FMAX", "F", "SF", "NORM_F", "ITL", "EFF" };

const uint SparkPropertyDataTypeType[] =
      { SDT_NO_PRINT, SDT_NO_PRINT, SDT_PREC, SDT_PREC, SDT_DOUBLE, SDT_PREC, SDT_PREC, SDT_PREC, SDT_PERCENT };


typedef enum
{
  SWP_TYPE = 0,
  SWP_FITNESS,
  SWP_BEST,
  SWP_ITER,
  SWP_INDEX,
  SWP_SHAPE_FITNESS,
  SWP_MAX,
} SwarmPropertyIndex;

const std::string SwarmPropertyTypeName[] =
      { "TYPE", "F", "BEST", "ITER", "INDEX", "SHAPE_FITNESS" };

const uint SwarmPropertyDataTypeType[] =
      { SDT_ELEMENTTYPENAME, SDT_DOUBLE, SDT_DOUBLE, SDT_INT, SDT_INT, SDT_INT };

typedef enum
{
  SWT_BEST = 0,
  SWT_PARTICLE,
  SWT_VELO,
  SWT_SPARK,
  SWT_MAX,
} ElementTypeIndex;

const std::string ElementTypeName[] =
      { "BEST", "PARTICLE", "VELO", "SPARK" };

typedef enum
{
  SSP_FITNESS = 0,
  SSP_A,
  SSP_FWAINDEX,
  SSP_SPARKINDEX,
  SSP_FWAITER,
  SSP_POPITER,
  SSP_TYPE,
  SSP_SHAPE_FITNESS,
  SSP_MAX,
} ARRAY_SFWAPropertyIndex;

const std::string SFWAPropertyTypeName[] =
      { "F", "A", "FWAINDEX", "SPARKINDEX", "FWAITER", "POPITER", "TYPE", "SF" };

const uint SFWAPropertyDataTypeType[] =
      { SDT_DOUBLE, SDT_PREC, SDT_INT, SDT_INT, SDT_INT, SDT_INT, SDT_ELEMENTTYPENAME, SDT_INT };


typedef enum
{
  SSET_ID = 0,
  SSET_TYPE,
  SSET_STATE,
  SSET_SUBSTATE,
  SSET_INDEX,  //index of sparkVektor
  SSET_FW_INDEX,
  SSET_FW_POP_INDEX,
  SSET_RECORDINDEX, //index of sparkdata
  SSET_DMIN,
  SSET_DMAX,
  SSET_DIM,
  SSET_FWCOUNT,
  SSET_ITER,
  SSET_EQ_TERRAINCOUNT,
  SSET_TELEMETRIC_DIR,
  SSET_TERRAIN,
  SSET_TERRAIN_DIR,
  SSET_DOMSPEED_DIR,
  SSET_MAX,
} ARRAY_SparkStateTypeIndex;

const std::string SparkStateTypeName[] =
      { "ID", "TYPE", "STATE", "SUBSTATE", "INDEX", "FW_INDEX", "FW_POP_INDEX", "RECORDINDEX", "DMIN", "DMAX", "DIM", "FWCOUNT", "ITER", "EQTC", "TELEMETRIC_DIR", "TERRAIN_DIR", "TERRAIN", "DOMSPEED_DIR"
      };

const uint SparkStateDataTypeType[] =
      { SDT_INT, SDT_TYPENAME, SDT_STATENAME, SDT_NO_PRINT, SDT_INT, SDT_INT, SDT_INT, SDT_INT, SDT_NO_PRINT, SDT_NO_PRINT, SDT_INT, SDT_INT, SDT_INT, SDT_INT, SDT_TELEMETRIC_DIR, SDT_TERRAIN, SDT_TERRAIN_DIR, SDT_DOMSPEED_DIR
      };

#define VELO_WITH_REAL_FITNESS 1
typedef enum
{
  SSIV_VELO_FIRST = 0,
  SSIV_VELO_LAST = SH_LAST - SH_FIRST - 1 + VELO_WITH_REAL_FITNESS,
  SSIV_MAX,  // 5
} ARRAY_SparkVeloStatIndex;

typedef enum
{
  SSIA_ACCEL_FIRST = 0,
  SSIA_ACCEL_LAST = SSIV_VELO_LAST - SSIV_VELO_FIRST - 1,
  SSIA_MAX,  // 4
} ARRAY_SparkAccelStatIndex;

typedef enum
{
  SSCT_VELO_FIRST = 0,
  SSCT_VELO_LAST = SSIV_MAX - 1,
  SSCT_VELO_MAX,  // 5
  SSCT_ACCEL_FIRST = SSCT_VELO_MAX,
  SSCT_ACCEL_LAST = SSCT_ACCEL_FIRST + SSIA_MAX - 1,
  SSCT_ACCEL_MAX,  // 9
  SSCT_MAX = SSCT_ACCEL_MAX,
} ARRAY_SparkStatisticTypeIndex;

const std::string SparkStatisticTypeName[] =
      { "V1", "V2", "V3", "V4", "V5", "A1", "A2", "A3", "A4" };

const uint SparkStatisticDataTypeType[] =
      { SDT_PREC, SDT_PREC, SDT_PREC, SDT_PREC, SDT_PREC, SDT_PREC, SDT_PREC, SDT_PREC, SDT_PREC };

typedef enum
{
  ST_UNKNOWN = 0,
  ST_FIRST,
  ST_RANDOM = ST_FIRST,
  ST_GAUSSIAN,
  ST_FIREWORK,
  ST_QUANTUM,
  ST_BEST,
  ST_LAST = ST_BEST,
  ST_MAX,
} SparkType;

const std::string SparkTypeName[] =
      { "UNKNOWN", "RANDOM", "GAUSSIAN", "FIREWORK", "QUANTUM", "BEST" };

typedef enum
{
  SCPT_UNKNOWN_PLACEHOLDER = 0, SCPT_UNKNOWN_MAX,
  SCPT_RANDOM_CONDCOEF = 0, SCPT_RANDOM_AMAXCOEF, SCPT_RANDOM_MAX,
  SCPT_GAUSSIAN_SPARK_ID = 0, SCPT_GAUSSIAN_SPARK_TYPE, SCPT_GAUSSIAN_MAX,
  SCPT_FIREWORK_CONDCOEF = 0, SCPT_FIREWORK_MAX,
  SCPT_QUANTUM_BEST_FITNESS = 0, SCPT_QUANTUM_ALPHA, SCPT_QUANTUM_MAX,
  SCPT_BEST_PARTNER_ID = 0, SCPT_BEST_PARTNER_ITER, SCPT_BEST_PARTNER_TYPE,
                      SCPT_BEST_PARENT_PARTNER_ID, SCPT_BEST_PARENT_PARTNER_TYPE,
                      SCPT_BEST_RANDOM_REAL_AMAX, SCPT_BEST_RANDOM_FITNESS, SCPT_BEST_RANDOM_AMAX,
                      SCPT_BEST_MAX,
  SCPT_MAX = SCPT_UNKNOWN_MAX + SCPT_RANDOM_MAX + SCPT_GAUSSIAN_MAX + SCPT_FIREWORK_MAX + SCPT_BEST_MAX + SCPT_QUANTUM_MAX,
} ARRAY_SparkCustomPropertyTypeIndex;

// used for calculating the necessary record size
const uint LargestCustomProperty =
        std::max( SCPT_UNKNOWN_MAX,
        std::max( SCPT_RANDOM_MAX,
        std::max( SCPT_GAUSSIAN_MAX,
        std::max( SCPT_FIREWORK_MAX,
        std::max( SCPT_BEST_MAX, SCPT_QUANTUM_MAX )
        ) ) ) );

const uint SparkCustomPropertyStartType[] =
      {
/*U*/   0,
/*R*/   SCPT_UNKNOWN_MAX,
/*G*/   SCPT_UNKNOWN_MAX + SCPT_RANDOM_MAX,
/*F*/   SCPT_UNKNOWN_MAX + SCPT_RANDOM_MAX + SCPT_GAUSSIAN_MAX,
/*Q*/   SCPT_UNKNOWN_MAX + SCPT_RANDOM_MAX + SCPT_GAUSSIAN_MAX + SCPT_FIREWORK_MAX,
/*BE*/  SCPT_UNKNOWN_MAX + SCPT_RANDOM_MAX + SCPT_GAUSSIAN_MAX + SCPT_FIREWORK_MAX + SCPT_QUANTUM_MAX
      };

const uint SparkCustomPropertyTypeSize[] =
      {
        SCPT_UNKNOWN_MAX,
        SCPT_RANDOM_MAX,
        SCPT_GAUSSIAN_MAX,
        SCPT_FIREWORK_MAX,
        SCPT_QUANTUM_MAX,
        SCPT_BEST_MAX,
      };

const std::string SparkCustomPropertyTypeName[] =
      {
/*U*/   "PLACEHOLDER",
/*R*/   "CONDCOEF", "AMAXCOEF",
/*G*/   "SPARK_ID", "SPARK_TYPE",
/*F*/   "CONDCOEF",
/*Q*/   "BEST_FITNESS", "ALPHA",
/*BE*/  "PARTNER_ID", "PARTNER_ITER", "PARTNER_TYPE", "PP_ID", "PP_TYPE", "BR_REAL_AMAX", "BR_FITNESS", "BR_AMAX",
      };

const uint SparkCustomPropertyDataTypeType[] =
      {
/*U*/   SDT_NO_PRINT,
/*R*/   SDT_PERCENT, SDT_PREC,
/*G*/   SDT_INT, SDT_TYPENAME,
/*F*/   SDT_PERCENT,
/*Q*/   SDT_PREC, SDT_PREC,
/*BE*/  SDT_INT, SDT_INT, SDT_TYPENAME, SDT_INT, SDT_TYPENAME, SDT_PREC, SDT_PREC, SDT_PREC,
      };

typedef enum
{
/*U*/   SCLT_UNKNOWN_AMAX = 0, SCLT_UNKNOWN_MAX,
/*R*/   SCLT_RANDOM_AMAX = 0, SCLT_RANDOM_MAX,
/*G*/   SCLT_GAUSSIAN_AMAX = 0, SCLT_GAUSSIAN_MAX,
/*F*/   SCLT_FIREWORK_AMAX = 0, SCLT_FIREWORK_MAX,
/*Q*/   SCLT_QUANTUM_AMAX = 0, SCLT_QUANTUM_BESTPARAM, SCLT_QUANTUM_ALPHA, SCLT_QUANTUM_MAX,
/*BE*/  SCLT_BEST_AMAX = 0, SCLT_BEST_BESTRANDOM_REAL_AMAX, SCLT_BEST_SPARK_REAL_AMAX, SCLT_BEST_MAX,
        SCLT_MAX = SCLT_UNKNOWN_MAX + SCLT_RANDOM_MAX + SCLT_GAUSSIAN_MAX + SCLT_FIREWORK_MAX + SCLT_BEST_MAX + SCLT_QUANTUM_MAX,
} ARRAY_SparkCustomLocationTypeIndex;

const uint SparkCustomLocationStartType[] =
        {
/*U*/     0,
/*R*/     SCLT_UNKNOWN_MAX,
/*G*/     SCLT_UNKNOWN_MAX + SCLT_RANDOM_MAX,
/*F*/     SCLT_UNKNOWN_MAX + SCLT_RANDOM_MAX + SCLT_GAUSSIAN_MAX,
/*Q*/     SCLT_UNKNOWN_MAX + SCLT_RANDOM_MAX + SCLT_GAUSSIAN_MAX + SCLT_FIREWORK_MAX,
/*BE*/    SCLT_UNKNOWN_MAX + SCLT_RANDOM_MAX + SCLT_GAUSSIAN_MAX + SCLT_FIREWORK_MAX + SCLT_QUANTUM_MAX,
        };

const uint SparkCustomLocationTypeSize[] =
      {
        SCLT_UNKNOWN_MAX,
        SCLT_RANDOM_MAX,
        SCLT_GAUSSIAN_MAX,
        SCLT_FIREWORK_MAX,
        SCLT_QUANTUM_MAX,
        SCLT_BEST_MAX,
      };

const std::string SparkCustomLocationTypeName[] =
      {
/*U*/   "AMAX",
/*R*/   "AMAX",
/*G*/   "AMAX",
/*F*/   "AMAX",
/*Q*/   "AMAX", "BESTPARAM", "ALPHA",
/*BE*/  "AMAX", "BESTRANDOM_REAL_AMAX", "SPARK_REAL_AMAX",
      };

const uint SparkCustomLocationAmaxID[] =
      {
/*U*/   SCLT_UNKNOWN_AMAX,
/*R*/   SCLT_RANDOM_AMAX,
/*G*/   SCLT_GAUSSIAN_AMAX,
/*F*/   SCLT_FIREWORK_AMAX,
/*Q*/   SCLT_QUANTUM_AMAX,
/*BE*/  SCLT_BEST_AMAX,
      };

typedef enum
{
  SD_UNKNOWN = 0,
  SD_AVG_FORWARD,
  SD_FORWARD,
  SD_BACKWARD,
  SD_CHANGEDIR,
  SD_RANDOM,
  SD_MAX,
} SparkDirection;

const std::string SparkDirectionName[] =
      { "UNKNOWN", "AVG_FORWARD","FORWARD", "BACKWARD", "CHANGEDIR", "RANDOM" };

typedef enum
{
  SBT_UNKNOWN = 0,
  SBT_NORMAL,
  SBT_VIRTUAL,
  SBT_CHILD,
  SBT_MAX,
} SparkBornType;

typedef enum
{
  SCS_UNKNOWN = 0,
  SCS_BORN,
  SCS_INIT_BEST_EVALUATE,
  SCS_INIT_POST_CALCULATION,
  SCS_INIT_NEXT_STATE,
  SCS_FLOW,
  SCS_BEST_EVALUATE,
  SCS_POST_CALCULATION,
  SCS_NEXT_STATE,
  SCS_DIE,
  SCS_DIE_BEST_EVALUATE,
  SCS_DIE_POST_CALCULATION,
  SCS_DIE_NEXT_STATE,
  SCS_MAX,
} SparkCalculationState;

const std::string SparkCalculationStateName[] =
      {
        "UNKNOWN",
        "BORN",
        "INIT_BEST_EVALUATE", "INIT_POST_CALCULATION", "INIT_NEXT_STATE",
        "FLOW", "BEST_EVALUATE", "POST_CALCULATION", "NEXT_STATE",
        "DIE", "DIE_BEST_EVALUATE", "DIE_POST_CALCULATION", "DIE_NEXT_STATE"
      };

typedef enum
{
  SCSS_UNKNOWN = 0,
  SCSS_MAIN,
  SCSS_BEST_EVALUATE,
  SCSS_POST_CALCULATION,
  SCSS_NEXT_STATE,
  SCSS_MAX,
} SparkCalculationSubState;

typedef enum
{
  SCRS_UNKNOWN = 0,
  SCRS_NEXT,
  SCRS_AGAIN_BY_KILL,
  SCRS_MAX,
} SparkCalculationRepeatingState;


typedef enum
{
  SS_UNKNOWN = 0,
  SS_BORN,
  SS_FLOW,
  SS_DIE,
  SS_MAX,
} SparkState;

const std::string SparkStateName[] =
      { "UNKNOWN", "BORN", "FLOW", "DIE" };

typedef enum
{
  SSS_UNKNOWN = 0,
  SSS_START,
  SSS_RUN,
  SSS_FINISH,
  SSS_DYING,
  SSS_DIED,
  SSS_MAX,
} SparkSubState;

const std::string SparkSubStateName[] =
      { "UNKNOWN", "START", "RUN", "FINISH", "DYING", "DIED" };

typedef enum
{
  THREADTYPE_EMPTY = 0,
  THREADTYPE_LOG,
  THREADTYPE_SF,
  THREADTYPE_PF,
  THREADTYPE_MAX,
} ThreadType;

typedef enum
{
  TS_UNKNOWN = 0,
  TS_SLEEP,
  TS_RUN,
  TS_MAX,
} ThreadStatus;

typedef enum
{
  QT_UNKNOWN = 0,
  QT_nFWA,
  QT_PSO,
  QT_oFWA,
  QT_HEAT,
  QT_MAX,
} QueueType;

typedef enum
{
  STMS_NOT_CALCULATED = 0,
  STMS_UNKNOWN,

  STMS_V1mV2mAm,
  STMS_V1mV2mAn,
  STMS_V1mV2mAp,
  STMS_V1mV2nAp,
  STMS_V1mV2pAp,

  STMS_V1nV2mAm,
  STMS_V1nV2nAn,
  STMS_V1nV2pAp,

  STMS_V1pV2pAn,
  STMS_V1pV2pAp,
  STMS_V1pV2nAm,
  STMS_V1pV2mAm,
  STMS_V1pV2pAm,

  STMS_V1m,
  STMS_V1n,
  STMS_V1p,

  STMS_MAX,
} SparkTeleMetricSign;

const std::string SparkTeleMetricSignName[] =
      {
        "NOT_CALCULATED", "UNKNOWN", "V1mV2mAm", "V1mV2mAn", "V1mV2mAp", "V1mV2nAp", "V1mV2pAp",
        "V1nV2mAm", "V1nV2nAn", "V1nV2pAp", "V1pV2pAn", "V1pV2pAp", "V1pV2nAm", "V1pV2mAm", "V1pV2pAm", "V1m", "V1n", "V1p"
      };

typedef enum
{
  STMD_NOT_CALCULATED = 0,
  STMD_UNKNOWN,
  STMD_INC,
  STMD_INC_SLOWER,
  STMD_INC_FASTER,
  STMD_INC_NULL,
  STMD_NULL,
  STMD_DEC,
  STMD_DEC_NULL,
  STMD_DEC_SLOWER,
  STMD_DEC_FASTER,
  STMD_DOM_V1_DEC,
  STMD_DOM_V1_INC,
  STMD_MAX,
} SparkTelemetricDirection;

const std::string SparkTelemetricDirectionName[] =
      { "NOT_CALCULATED", "UNKNOWN", "INC", "INC_SLOWER", "INC_FASTER", "INC_NULL",
        "NULL", "DEC", "DEC_NULL", "DEC_SLOWER", "DEC_FASTER", "DOM_V1_DEC", "DOM_V1_INC" };

typedef enum
{
  SDOM_NOT_CALCULATED = 0,
  SDOM_UNKNOWN,
  SDOM_DEC,
  SDOM_EQ,
  SDOM_INC,
  SDOM_MAX,
} SparkDominantSpeedDirection;

const std::string SparkDominantSpeedDirectionName[] =
      { "NOT_CALCULATED", "UNKNOWN", "DEC", "EQ", "INC" };

typedef enum
{
  SET_NOT_CALCULATED = 0,
  SET_UNKNOWN,
  SET_DEC_SLOWER,
  SET_DEC_FASTER,
  SET_DEC_EQ,
  SET_DEC,
  SET_EQ,
  SET_INC,
  SET_INC_EQ,
  SET_INC_SLOWER,
  SET_INC_FASTER,
  SET_DOM_V1_DEC,
  SET_DOM_V1_EQ,
  SET_DOM_V1_INC,
  SET_DOM_V2_DEC,
  SET_DOM_V2_EQ,
  SET_DOM_V2_INC,
  SET_MAX,
} SparkTerrain;

const std::string SparkTerrainName[] =
      { "NOT_CALCULATED", "UNKNOWN", "DEC_SLOWER", "DEC_FASTER", "DEC_EQ", "DEC", "EQ",
        "INC", "INC_EQ", "INC_SLOWER", "INC_FASTER", "DOM_V1_DEC",
        "DOM_V1_EQ", "DOM_V1_INC", "DOM_V2_DEC", "DOM_V2_EQ", "DOM_V2_INC" };

typedef enum
{
  SCT_NOT_CALCULATED = 0,
  SCT_UNKNOWN,
  SCT_DEC,
  SCT_EQ,
  SCT_INC,
  SCT_MAX,
} SparkTerrainDirection;

const std::string SparkTerrainDirectionName[] =
      { "NOT_CALCULATED", "UNKNOWN", "DEC", "EQ", "INC" };

// these enums order is mandatory because these order uses
// the preparelogging and logcreator functions!
typedef enum
{
  LSAAI_PARAMS = 0,
  LSAAI_PROPERTY,
  LSAAI_STATISTICS,
  LSAAI_CUSTOMS,
  LSAAI_CALCFUNCTIONVALUE,
  LSAAI_FITNESSFUNCTION,
  LSAAI_REALAMAXFUNCTION,
  LSAAI_MAX
} LogSparkPrecArrayArgsIndex;

typedef enum
{
  LOSAAI_PARAMS = 0,
  LOSAAI_PROPERTY,
  LOSAAI_CALCFUNCTIONVALUE,
  LOSAAI_MAX
} LogOldSparkPrecArrayArgsIndex;

typedef enum
{
  LSLAAI_CUSTOMLOCATION = 0,
  LSLAAI_MAX
} LogSparkLArrayArgsIndex;

typedef enum
{
  LSLCAAI_STATES = 0,
  LSLCAAI_MAX
} LogSparkCountArrayArgsIndex;

typedef enum
{
  LSPAAI_PARAMS = 0,
  LSPAAI_VELO,
  LSPAAI_PROPERTY,
  LSPAAI_CALCFUNCTIONVALUE,
  LSPAAI_MAX
} LogSwarmArrayArgsIndex;

typedef struct _heatCalcParams
{
  // temporaty store for calculation
  PREC* temperatures = NULL;
  PREC* nextTemperatures = NULL;

  PREC* heatFunctionValues = NULL;
  PREC* fitnessFunctions = NULL;
  PREC* referenceFunctionTimes = NULL;
  PREC* referenceFunctionValues = NULL;
  PREC* HTCTimeFunction = NULL;

  PREC R = 0;
  PREC endTime = 0;
  PREC initialTemperature = 0;
  PREC finalTemperature = 0;

  COUNT_T N = 0;
  COUNT_T htcN = 0;
  COUNT_T TCIndex = 0;
  COUNT_T referenceFunctionN = 0;
  COUNT_T lastTemperatureIndex = 0;
  COUNT_T temperatureLength = 0;

  PREC PREC_referenceFunctionN = 0;
  PREC PREC_lastTemperatureIndex = 0;
  PREC PREC_N = 0;

  // const pre-calculated values for calculation - CONST
  PREC dx = 0;
  PREC dx2 = 0;
  PREC dx4 = 0;
  PREC stability_dt = 0;
  PREC min_dt = 0;
  PREC delta_t = 0;
  PREC dtpdx2 = 0;
  PREC dtdx2m2 = 0;
  PREC decTmod = 0;

  PREC* c1pc1p2i = NULL;
  PREC* c1mc1p2i = NULL;

  // stores for quick calculations
  // inputs
  COUNT_T    start_iteration_index = 0; // start index of referenceFunctionTimes
  COUNT_T    next_iteration_index = 0;  // next_start index of referenceFunctionTimes

  // input
  PREC* start_iteration_temperatures = NULL; // temperature of referenceFunctionValues at start_iteration_index point
                                                  // length is N, values are come from PREC* temperatures
  // output
  PREC* next_iteration_temperatures = NULL;  // temperature of referenceFunctionValues at start_iteration_index point
                                                  // length is N, values are come from PREC* temperatures

  PREC* interpolated_htc = NULL;
} HeatCalcParams;

typedef enum
{
  SA_nFWA = 0,
  SA_FWA,
  SA_cFWA,
  SA_AFWA,
  SA_EFWA,
  SA_EFWADM,
  SA_FWAMAX,
  SA_PSO,
  SA_PSOCo,
  SA_PSOIn,
  SA_QPSOT1,
  SA_QPSOT2,
  SA_PSOMAX,
  SA_ALLFWA,
  SA_ALLPSO,
  SA_MAX,
} SolverAlgs;

const std::string solver_algsName[] =
      {
        "nFWA", "FWA", "cFWA", "AFWA", "EFWA", "EFWADM",
        "FWAMAX",
        "PSO", "PSOCo", "PSOIn", "QPSOT1", "QPSOT2",
        "PSOMAX",
        "ALLFWA", "ALLPSO"
      };
typedef enum
{
  SF_UNKNOWN = 0,
  SF_HTC,
  SF_SIMPLE,
  SF_MAX
} ShapeFunction;

typedef enum
{
  PF_REF = 0,
  PF_DEV_ERR,
  PF_DEV_INTERP,
  PF_DEV_HTCGEN,
  PF_DEV_QUICK_CC,
  PF_TECH_MAX,
  PF_SPEED,
  PF_HCP,
  PF_Sphere,
  PF_Scwefel12,
  PF_Rosenbrock,
  PF_SAckley,
  PF_GGriewank,
  PF_RHEll,
  PF_Rastigin,
  PF_APHEll,
  PF_ALLTEST,
  PF_MAX,
} ProblemFunctions;

const std::string problem_funcsName[] =
      {
        "REF",
        "DEV_ERR", "DEV_INTERP", "DEV_HTCGEN", "DEV_QUICK_CC",
        "TECH_MAX",
        "SPEED", "HCP",
        "Sphere", "Scwefel12", "Rosenbrock", "SAckley", "GGriewank", "RHEll", "Rastigin", "APHEll",
        "ALLTEST"
      };

typedef struct _CalculationResults
{
  PREC fitness = 0;
  PREC shapefitness = 0;
  COUNT_T iteration = 0;
  COUNT_T calculation = 0;
  std::string msg = "";
  long ts = 0;
} CalculationResults;

typedef std::vector< CalculationResults* > TResults;

typedef struct _PFTestResults
{
  std::string pf_name = "unknown";
  std::string sa_name = "unknown";
  TResults* result;
} PFTestResults;

typedef std::vector< PFTestResults* > TPFTestResults;
typedef std::vector< std::ofstream* > TFilestreamList;

typedef enum
{
  LL_NO = 0,
  LL_BEST = 1,
  LL_BESTS = 2,
  LL_SPARK = 4,
  LL_PARTICLE = 4,
  LL_MAX = 8,
} LogLevel;

const std::string LogLevelName[] =
      {
        "no",
        "best",
        "bests",
        "all"
      };

typedef enum
{
  LT_NO = 0,
  LT_BEST,
  LT_SPARK,
  LT_MAX,
} LogType;

typedef enum
{
  CSMMH_UNKNOWN = 0,
  CSMMH_DOUBLE,
  CSMMH_LONG,
  CSMMH_COUNT,
  CSMMH_MAX,
} CliStatsMinMaxHelper;

typedef enum
{
  DT_UNKNOWN = 0,
  DT_PREC,
  DT_COUNT,
  DT_INT,
  DT_MAX,
} DataType;

const std::unordered_map< std::type_index, uint > typeID =
{
  { std::type_index( typeid( uint32_t ) ) , DT_COUNT },
  { std::type_index( typeid( uint64_t ) ) , DT_COUNT },
  { std::type_index( typeid( double   ) ) , DT_PREC  },
  { std::type_index( typeid( float    ) ) , DT_PREC  },
  { std::type_index( typeid( long     ) ) , DT_COUNT },
  { std::type_index( typeid( uint     ) ) , DT_COUNT },
  { std::type_index( typeid( int      ) ) , DT_INT   },
};

typedef struct _DevFunctionParameters
{
  std::string htcTimeFile = "";
  std::string htcValueFile = "";
  std::string coolingCurveTimeFile = "";
  std::string coolingCurveValueFile = "";
  std::string outputFile = "";
} DevFunctionParameters;

typedef enum
{
  OCT_UNKNOWN = 0,
  OCT_DIRECTED_RANDOM,
  OCT_AMAX_RANDOM,
  OCT_DIRECTED,
  OCT_SHAPE,
  OCT_MAX,
} OffsetCalculationType;

typedef struct _boundaries
{
  PREC Hmin = 0;
  PREC Hmax = 0;
  COUNT_T Dim = 0;
  COUNT_T Dmin = 0;
  COUNT_T Dmax = 0;
  COUNT_T fw_count = 0;
  COUNT_T random_spark_count = 0;
  COUNT_T gaussian_spark_count = 0;
  COUNT_T quantum_spark_count = 0;
  COUNT_T fw_max_iter = 0;
  COUNT_T spark_max_iter = 0;
  COUNT_T qcurve_point_size = 0;
  PREC min_fitness = 0;
  COUNT_T shapeFunction = SF_UNKNOWN;
} Boundaries;
