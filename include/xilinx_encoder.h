#ifdef __cplusplus
extern "C" {
#endif

#ifndef _API_H_
#define _API_H_

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <dlfcn.h>
#include <regex.h>
#include <xrm.h>
#include <xmaplugin.h>
#include <xma.h>
#include <xvbm.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <termios.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include <time.h>
#include <errno.h>

#define FLAG_HELP             "help"
#define FLAG_DEVICE_ID        "d"
#define FLAG_STREAM_LOOP      "stream_loop"
#define FLAG_INPUT_FILE       "i"
#define FLAG_CODEC_TYPE       "c:v"
#define FLAG_INPUT_WIDTH      "w"
#define FLAG_INPUT_HEIGHT     "h"
#define FLAG_INPUT_PIX_FMT    "pix_fmt"
#define FLAG_BITRATE          "b:v"
#define FLAG_BIT_RATE         "b"
#define FLAG_FPS              "fps"
#define FLAG_INTRA_PERIOD     "g"
#define FLAG_CONTROL_RATE     "control-rate"
#define FLAG_MAX_BITRATE      "max-bitrate"
#define FLAG_SLICE_QP         "slice-qp"
#define FLAG_MIN_QP           "min-qp"
#define FLAG_MAX_QP           "max-qp"
#define FLAG_NUM_BFRAMES      "bf"
#define FLAG_IDR_PERIOD       "periodicity-idr"
#define FLAG_PROFILE          "profile"
#define FLAG_LEVEL            "level"
#define FLAG_NUM_SLICES       "slices"
#define FLAG_QP_MODE          "qp-mode"
#define FLAG_ASPECT_RATIO     "aspect-ratio"
#define FLAG_SCALING_LIST     "scaling-list"
#define FLAG_LOOKAHEAD_DEPTH  "lookahead-depth"
#define FLAG_TEMPORAL_AQ      "temporal-aq"
#define FLAG_SPATIAL_AQ       "spatial-aq"
#define FLAG_SPATIAL_AQ_GAIN  "spatial-aq-gain"
#define FLAG_QP               "qp"
#define FLAG_NUM_FRAMES       "frames"
#define FLAG_NUM_CORES        "cores"
#define FLAG_TUNE_METRICS     "tune-metrics"
#define FLAG_LATENCY_LOGGING  "latency_logging"
#define FLAG_OUTPUT_FILE      "o"

typedef struct {
    xrmContext*       xrm_ctx;
    xrmCuListResource encode_cu_list_res;
    xrmCuResource     lookahead_cu_res;
    int32_t           device_id;
    int32_t           enc_res_idx;
    int32_t           enc_load;
    int32_t           la_load;
    int32_t           enc_num;
    int32_t           enc_res_in_use;
    int32_t           lookahead_res_inuse;
} XlnxEncoderXrmCtx;

/* HEVC Encoder supported profiles */
typedef enum
{
    ENC_HEVC_MAIN = 0,
    ENC_HEVC_MAIN_INTRA
} XlnxHevcProfiles;

typedef enum
{
    ENCODER_ID_H264 = 0,
    ENCODER_ID_HEVC
} XlnxEncoderCodecID;

typedef enum
{
    LOOKAHEAD_ID_H264 = 0,
    LOOKAHEAD_ID_HEVC
} XlnxLookaheadCodecID;

typedef enum
{
    YUV_NV12_ID = 0,
    YUV_420P_ID

} XlnxInputPixelFormat;

typedef enum
{
    HELP_ARG = 0,
    DEVICE_ID_ARG,
    LOOP_COUNT_ARG,
    INPUT_FILE_ARG,
    ENCODER_ARG,
    INPUT_WIDTH_ARG,
    INPUT_HEIGHT_ARG,
    INPUT_PIX_FMT_ARG,
    BITRATE_ARG,
    FPS_ARG,
    INTRA_PERIOD_ARG,
    CONTROL_RATE_ARG,
    MAX_BITRATE_ARG,
    SLICE_QP_ARG,
    MIN_QP_ARG,
    MAX_QP_ARG,
    NUM_BFRAMES_ARG,
    IDR_PERIOD_ARG,
    PROFILE_ARG,
    LEVEL_ARG,
    NUM_SLICES_ARG,
    QP_MODE_ARG,
    ASPECT_RATIO_ARG,
    SCALING_LIST_ARG,
    LOOKAHEAD_DEPTH_ARG,
    TEMPORAL_AQ_ARG,
    SPATIAL_AQ_ARG,
    SPATIAL_AQ_GAIN_ARG,
    QP_ARG,
    NUM_FRAMES_ARG,
    LATENCY_LOGGING_ARG,
    NUM_CORES_ARG,
    TUNE_METRICS_ARG,
    OUTPUT_FILE_ARG
} XlnxEncArgIdentifiers;

typedef enum
{
    EParamIntraPeriod = 0,
    EParamLADepth,
    EParamEnableHwInBuf,
    EParamSpatialAQMode,
    EParamTemporalAQMode,
    EParamRateControlMode,
    EParamSpatialAQGain,
    EParamNumBFrames,
    EParamCodecType,
    EParamLatencyLogging
} XlnxLAExtParams;

typedef enum {

    ENC_READ_INPUT = 0,
    ENC_LA_PROCESS,
    ENC_LA_FLUSH,
    ENC_SEND_INPUT,
    ENC_GET_OUTPUT,
    ENC_FLUSH,
    ENC_EOF,
    ENC_STOP,
    ENC_DONE
} XlnxEncoderState;

typedef struct
{
    char *key;
    int value;
} XlnxEncProfileLookup;

typedef struct
{
    XmaFormatType      xma_fmt_type;
    XmaFraction        framerate;
    XlnxLookaheadCodecID   codec_type;
    int32_t            width; 
    int32_t            height;
    int32_t            stride;
    int32_t            bits_per_pixel;
    int32_t            gop_size;
    uint32_t           lookahead_depth;
    uint32_t           spatial_aq_mode;
    uint32_t           temporal_aq_mode;
    uint32_t           rate_control_mode;
    uint32_t           spatial_aq_gain;
    uint32_t           num_bframes;
    int32_t            latency_logging;
    uint8_t            enable_hw_buf;
} XlnxLookaheadProperties;

typedef struct
{
    XmaFilterSession   *filter_session;
    XmaFrame           *xma_la_frame;
    XlnxLookaheadProperties la_props;
    int32_t            num_planes;
    uint8_t            bypass;
} XlnxLookaheadCtx;

typedef struct {
    double cpb_size;
    double initial_delay;
    int64_t max_bitrate;
    int64_t bit_rate;
    int32_t width;
    int32_t height;
    int32_t pix_fmt;
    int32_t fps;
    int32_t gop_size;
    int32_t slice_qp;
    int32_t min_qp;  
    int32_t max_qp;
    int32_t codec_id;
    int32_t control_rate;
    int32_t custom_rc;
    int32_t gop_mode;
    int32_t gdr_mode;
    uint32_t num_bframes;
    uint32_t idr_period;
    int32_t profile;
    int32_t level; 
    int32_t tier;
    int32_t num_slices;
    int32_t dependent_slice;
    int32_t slice_size;
    int32_t lookahead_depth;
    int32_t temporal_aq;
    int32_t spatial_aq;
    int32_t spatial_aq_gain;
    int32_t qp_mode;
    int32_t filler_data;
    int32_t aspect_ratio;
    int32_t scaling_list;
    int32_t entropy_mode;
    int32_t loop_filter;
    int32_t constrained_intra_pred;
    int32_t prefetch_buffer;
    int32_t tune_metrics;
    int32_t num_cores;
    int32_t latency_logging;
    uint32_t enable_hw_buf;
    char    *enc_options;
}XlnxEncoderProperties;

/* Encoder Context */
typedef struct {
    XmaDataBuffer         xma_buffer;
    XmaFrame              in_frame;
    XmaEncoderSession     *enc_session;
    XmaFrame              *enc_in_frame;
    XmaFrame              *la_in_frame;
    XlnxEncoderXrmCtx     enc_xrm_ctx;
    XlnxLookaheadCtx      la_ctx;
    XlnxEncoderProperties enc_props;
    size_t                num_frames;
    size_t                in_frame_cnt;
    size_t                out_frame_cnt;
    int32_t               loop_count;
    uint32_t              la_bypass;
    uint32_t              enc_state;
    int32_t               pts;
    FILE                  *in_file;
    FILE                  *out_file;
} XlnxEncoderCtx;

#define ENC_APP_SUCCESS            0
#define ENC_APP_FAILURE            (-1)
#define ENC_APP_DONE               1
#define ENC_APP_STOP               2

#define ENC_DEFAULT_NUM_B_FRAMES   2
#define ENC_DEFAULT_LEVEL          10
#define ENC_DEFAULT_FRAMERATE      25
#define ENC_DEFAULT_SPAT_AQ_GAIN   50
#define ENC_MAX_SPAT_AQ_GAIN       100
#define ENC_DEFAULT_GOP_SIZE       120

#define MAX_ARG_SIZE               64
#define VCU_HEIGHT_ALIGN           64
#define VCU_STRIDE_ALIGN           256

#define ENC_MIN_LOOKAHEAD_DEPTH    0
#define ENC_MAX_LOOKAHEAD_DEPTH    20

#define ENC_SUPPORTED_MIN_WIDTH    64
#define ENC_DEFAULT_WIDTH          1920
#define ENC_SUPPORTED_MAX_WIDTH    3840
#define ENC_MAX_LA_INPUT_WIDTH     1920

#define ENC_SUPPORTED_MIN_HEIGHT   64
#define ENC_DEFAULT_HEIGHT         1080
#define ENC_SUPPORTED_MAX_HEIGHT   2160
#define ENC_MAX_LA_INPUT_HEIGHT    1080

#define XLNX_ENC_LINE_ALIGN(x,LINE_SIZE) (((((size_t)x) + \
                    ((size_t)LINE_SIZE - 1)) & (~((size_t)LINE_SIZE - 1))))

#define ENC_SUPPORTED_MAX_PIXELS   ((ENC_SUPPORTED_MAX_WIDTH) * (ENC_SUPPORTED_MAX_HEIGHT))

#define ENC_MAX_LA_PIXELS          ((ENC_MAX_LA_INPUT_WIDTH) * (ENC_MAX_LA_INPUT_HEIGHT))

#define ENC_DEFAULT_BITRATE        5000
#define ENC_DEFAULT_MAX_BITRATE    (ENC_DEFAULT_BITRATE)
#define ENC_SUPPORTED_MAX_BITRATE  35000000

#define ENC_SUPPORTED_MIN_QP       0
#define ENC_SUPPORTED_MAX_QP       51

#define ENC_OPTION_DISABLE         0
#define ENC_OPTION_ENABLE          1

#define ENC_RC_CONST_QP_MODE       0
#define ENC_RC_CBR_MODE            1
#define ENC_RC_VBR_MODE            2
#define ENC_RC_LOW_LATENCY_MODE    3

#define ENC_DEFAULT_GOP_MODE       0
#define ENC_PYRAMIDAL_GOP_MODE     1
#define ENC_LOW_DELAY_P_MODE       2
#define ENC_LOW_DELAY_B_MODE       3

#define ENC_GDR_DISABLE            0
#define ENC_GDR_VERTICAL_MODE      1
#define ENC_GDR_HORIZONTAL_MODE    2

#define ENC_LEVEL_10               10
#define ENC_LEVEL_11               11
#define ENC_LEVEL_12               12
#define ENC_LEVEL_13               13
#define ENC_LEVEL_20               20
#define ENC_LEVEL_21               21
#define ENC_LEVEL_22               22
#define ENC_LEVEL_30               30
#define ENC_LEVEL_31               31
#define ENC_LEVEL_32               32
#define ENC_LEVEL_40               40
#define ENC_LEVEL_41               41
#define ENC_LEVEL_42               42
#define ENC_LEVEL_50               50
#define ENC_LEVEL_51               51
#define ENC_LEVEL_52               52

#define ENC_UNIFORM_QP_MODE        0
#define ENC_AUTO_QP_MODE           1
#define ENC_RELATIVE_LOAD_QP_MODE  2

#define ENC_ASPECT_RATIO_AUTO      0
#define ENC_ASPECT_RATIO_4_3       1
#define ENC_ASPECT_RATIO_16_9      2
#define ENC_ASPECT_RATIO_NONE      3

#define ENC_CAVLC_MODE             0
#define ENC_CABAC_MODE             1

#define ENC_MAX_OPTIONS_SIZE       2048
#define ENC_MAX_EXT_PARAMS         3

/* H264 Encoder supported profiles */
#define ENC_H264_BASELINE          66
#define ENC_H264_MAIN              77
#define ENC_H264_HIGH              100

#define XLNX_ENC_APP_MODULE        "xlnx_encoder_app"

/* Lookahead constants */
#define XLNX_SCLEVEL1              2
#define XLNX_LA_MAX_NUM_EXT_PARAMS 10
#define XLNX_MAX_LOOKAHEAD_DEPTH   20




#define DEFAULT_DEVICE_ID    -1
#define XMA_PROPS_TO_JSON_SO "/opt/xilinx/xrm/plugin/libxmaPropsTOjson.so"
#define XCLBIN_PARAM_NAME    "/opt/xilinx/xcdr/xclbins/transcode.xclbin"

#define RET_ERROR     XMA_ERROR
#define RET_SUCCESS   XMA_SUCCESS
#define RET_EOF       XMA_SUCCESS + 1
#define RET_EOS       RET_EOF + 1

#define STRIDE_ALIGN  256
#define HEIGHT_ALIGN  64
#define ALIGN(x,align) (((x) + (align) - 1) & ~((align) - 1))

#define UNASSIGNED               INT32_MIN
#define replace_if_unset(a,b)    ((a == UNASSIGNED) ? (b) : (a))

#define H265_CODEC_TYPE 1
#define HEVC_CODEC_TYPE H265_CODEC_TYPE
#define H265_CODEC_NAME "mpsoc_vcu_hevc"
#define HEVC_CODEC_NAME H265_CODEC_NAME

#define H264_CODEC_TYPE 0
#define AVC_CODEC_TYPE  H264_CODEC_TYPE
#define H264_CODEC_NAME "mpsoc_vcu_h264"
#define AVC_CODEC_NAME  H264_CODEC_NAME

#define DEBUG_LOGLEVEL           0
#define XLNX_APP_UTILS_MODULE    "xlnx_app_utils"
#define XLNX_APP_UTILS_LOG_ERROR(msg...) \
            xma_logmsg(XMA_ERROR_LOG, XLNX_APP_UTILS_MODULE, msg)
#define XLNX_APP_UTILS_LOG_INFO(msg...) \
            xma_logmsg(XMA_INFO_LOG, XLNX_APP_UTILS_MODULE, msg)

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#define XRM_PRECISION_1000000_BIT_MASK(load) ((load << 8))

static const char *XLNX_LA_EXT_PARAMS[] = {
    "ip",
    "lookahead_depth",
    "enable_hw_in_buf",
    "spatial_aq_mode",
    "temporal_aq_mode",
    "rate_control_mode",
    "spatial_aq_gain",
    "num_b_frames",
    "codec_type",
    "latency_logging"
};

#define DEFAULT_DEVICE_ID    -1
#define XMA_PROPS_TO_JSON_SO "/opt/xilinx/xrm/plugin/libxmaPropsTOjson.so"
#define XCLBIN_PARAM_NAME    "/opt/xilinx/xcdr/xclbins/transcode.xclbin"

#define RET_ERROR     XMA_ERROR
#define RET_SUCCESS   XMA_SUCCESS
#define RET_EOF       XMA_SUCCESS + 1
#define RET_EOS       RET_EOF + 1

#define STRIDE_ALIGN  256
#define HEIGHT_ALIGN  64
#define ALIGN(x,align) (((x) + (align) - 1) & ~((align) - 1))

#define MAX_DEC_PARAMS             11
#define MAX_DEC_WIDTH              3840
#define MAX_DEC_HEIGHT             2160

#define DEFAULT_ENTROPY_BUFF_COUNT 2
#define MIN_ENTROPY_BUFF_COUNT     2
#define MAX_ENTROPY_BUFF_COUNT     10

#define DEC_APP_ERROR              XMA_ERROR
#define DEC_APP_SUCCESS            XMA_SUCCESS

#define XLNX_DEC_APP_MODULE     "xlnx_decoder"
#define XRM_PRECISION_1000000_BIT_MASK(load) ((load << 8))
#define DECODER_APP_LOG_ERROR(msg...) \
            xma_logmsg(XMA_ERROR_LOG, XLNX_DEC_APP_MODULE, msg)
#define DECODER_APP_LOG_INFO(msg...) \
            xma_logmsg(XMA_INFO_LOG, XLNX_DEC_APP_MODULE, msg)

typedef struct XlnxDecoderProperties
{
    int32_t  device_id; // -1 by default
    uint32_t width;
    uint32_t height;
    uint32_t fps;
    uint32_t log_level;
    uint32_t bit_depth;
    uint32_t codec_type;
    uint32_t low_latency;
    uint32_t entropy_buf_cnt;
    uint32_t zero_copy;
    uint32_t profile_idc;
    uint32_t level_idc;
    uint32_t chroma_mode;
    uint32_t scan_type;
    uint32_t latency_logging;
    uint32_t splitbuff_mode;
} XlnxDecoderProperties;

typedef struct XlnxDecoderXrmCtx
{
    int                       xrm_reserve_id;
    int                       dec_load;
    int                       decode_res_in_use;
    xrmContext*               xrm_ctx;
    xrmCuListResource         decode_cu_list_res;
} XlnxDecoderXrmCtx;

typedef struct XlnxDecoderChannelCtx
{
    FILE*           out_fp;
    size_t          num_frames_to_decode;
    XmaFrame*       xframe;
} XlnxDecoderChannelCtx;

typedef struct XlnxDecoderCtx
{
    int32_t                   pts;
    bool                      is_flush_sent;
    size_t                    num_frames_sent;
    size_t                    num_frames_decoded;
    XmaDecoderSession*        xma_dec_session;
    XmaDecoderProperties      dec_xma_props;
    XlnxDecoderProperties     dec_params;
	XlnxDecoderChannelCtx     channel_ctx;
    XlnxDecoderXrmCtx         dec_xrm_ctx;
} XlnxDecoderCtx;

int Encoder_Init(XlnxEncoderCtx* enc_ctx,XmaEncoderProperties* xma_enc_props,XmaFilterProperties* xma_la_props);

int Encoder_frame(XlnxEncoderCtx* enc_ctx,char* iyBuf,char* iuvBuf,char* outBuf,int* outlen);

void Encoder_Release(XlnxEncoderCtx *enc_ctx,XmaEncoderProperties *xma_enc_props,XmaFilterProperties *xma_la_props);

void FPGA_Init();
#endif 

#ifdef __cplusplus
}
#endif