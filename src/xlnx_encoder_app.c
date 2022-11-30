#include "xilinx_encoder.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <getopt.h>
#include <dlfcn.h>


static int32_t xlnx_enc_xma_params_update(XlnxEncoderProperties *enc_props, 
                                          XmaEncoderProperties *xma_enc_props)
{
    int32_t param_cnt = 0;

    xma_enc_props->params[param_cnt].name   = "enc_options";
    xma_enc_props->params[param_cnt].type   = XMA_STRING;
    xma_enc_props->params[param_cnt].length = strlen(enc_props->enc_options);
    xma_enc_props->params[param_cnt].value  = &(enc_props->enc_options);
    param_cnt++;

    xma_enc_props->params[param_cnt].name   = "latency_logging";
    xma_enc_props->params[param_cnt].type   = XMA_UINT32;
    xma_enc_props->params[param_cnt].length = 
                                      sizeof(enc_props->latency_logging);
    xma_enc_props->params[param_cnt].value  = &(enc_props->latency_logging);
    param_cnt++;

    xma_enc_props->params[param_cnt].name = "enable_hw_in_buf";
    xma_enc_props->params[param_cnt].type = XMA_UINT32;
    xma_enc_props->params[param_cnt].length = 
                                      sizeof(enc_props->enable_hw_buf);
    xma_enc_props->params[param_cnt].value  = &enc_props->enable_hw_buf;
    param_cnt++;

    return ENC_APP_SUCCESS;

}

static void xlnx_la_xma_params_update(XlnxLookaheadProperties *la_props, 
                                      XmaFilterProperties *xma_la_props)
{

    XmaParameter *extn_params = NULL;
    uint32_t param_cnt = 0;
    extn_params = (XmaParameter *)calloc(1, 
                  XLNX_LA_MAX_NUM_EXT_PARAMS * sizeof(XmaParameter));

    extn_params[param_cnt].name = 
                            (char *)XLNX_LA_EXT_PARAMS[EParamIntraPeriod];
    extn_params[param_cnt].user_type = EParamIntraPeriod;
    extn_params[param_cnt].type = XMA_UINT32;
    extn_params[param_cnt].length = sizeof(int);
    extn_params[param_cnt].value = &la_props->gop_size;
    param_cnt++;

    extn_params[param_cnt].name = (char *)XLNX_LA_EXT_PARAMS[EParamLADepth];
    extn_params[param_cnt].user_type = EParamLADepth;
    extn_params[param_cnt].type = XMA_UINT32;
    extn_params[param_cnt].length = sizeof(int);
    extn_params[param_cnt].value = &la_props->lookahead_depth;
    param_cnt++;

    extn_params[param_cnt].name = 
                            (char *)XLNX_LA_EXT_PARAMS[EParamEnableHwInBuf];
    extn_params[param_cnt].user_type = EParamEnableHwInBuf;
    extn_params[param_cnt].type = XMA_UINT32;
    extn_params[param_cnt].length = sizeof(int);
    extn_params[param_cnt].value = &la_props->enable_hw_buf;
    param_cnt++;

    extn_params[param_cnt].name = 
                            (char *)XLNX_LA_EXT_PARAMS[EParamSpatialAQMode];
    extn_params[param_cnt].user_type = EParamSpatialAQMode;
    extn_params[param_cnt].type = XMA_UINT32;
    extn_params[param_cnt].length = sizeof(int);
    extn_params[param_cnt].value = &la_props->spatial_aq_mode;
    param_cnt++;

    extn_params[param_cnt].name = 
        (char *)XLNX_LA_EXT_PARAMS[EParamTemporalAQMode];
    extn_params[param_cnt].user_type = EParamTemporalAQMode;
    extn_params[param_cnt].type = XMA_UINT32;
    extn_params[param_cnt].length = sizeof(int);
    extn_params[param_cnt].value = &la_props->temporal_aq_mode;
    param_cnt++;

    extn_params[param_cnt].name = 
        (char *)XLNX_LA_EXT_PARAMS[EParamRateControlMode];
    extn_params[param_cnt].user_type = EParamRateControlMode;
    extn_params[param_cnt].type = XMA_UINT32;
    extn_params[param_cnt].length = sizeof(int);
    extn_params[param_cnt].value = &la_props->rate_control_mode;
    param_cnt++;

    extn_params[param_cnt].name = 
                            (char *)XLNX_LA_EXT_PARAMS[EParamSpatialAQGain];
    extn_params[param_cnt].user_type = EParamSpatialAQGain;
    extn_params[param_cnt].type = XMA_UINT32;
    extn_params[param_cnt].length = sizeof(int);
    extn_params[param_cnt].value = &la_props->spatial_aq_gain;
    param_cnt++;

    extn_params[param_cnt].name = 
                            (char *)XLNX_LA_EXT_PARAMS[EParamNumBFrames];
    extn_params[param_cnt].user_type = EParamNumBFrames;
    extn_params[param_cnt].type = XMA_UINT32;
    extn_params[param_cnt].length = sizeof(int);
    extn_params[param_cnt].value = &la_props->num_bframes;
    param_cnt++;

    extn_params[param_cnt].name = (char *)XLNX_LA_EXT_PARAMS[EParamCodecType];
    extn_params[param_cnt].user_type = EParamCodecType;
    extn_params[param_cnt].type = XMA_UINT32;
    extn_params[param_cnt].length = sizeof(int);
    extn_params[param_cnt].value = &la_props->codec_type;
    param_cnt++;

    extn_params[param_cnt].name = 
        (char *)XLNX_LA_EXT_PARAMS[EParamLatencyLogging];
    extn_params[param_cnt].user_type = EParamLatencyLogging;
    extn_params[param_cnt].type = XMA_UINT32;
    extn_params[param_cnt].length = sizeof(int);
    extn_params[param_cnt].value = &la_props->latency_logging;
    param_cnt++;

    xma_la_props->param_cnt = param_cnt;
    xma_la_props->params = &extn_params[0];

    return;
}

int32_t xlnx_enc_get_xma_props(XlnxEncoderProperties *enc_props, 
                               XmaEncoderProperties *xma_enc_props)
{

    /* Initialize encoder properties */
    xma_enc_props->hwencoder_type = XMA_MULTI_ENCODER_TYPE;
    strcpy(xma_enc_props->hwvendor_string, "MPSoC");
    xma_enc_props->param_cnt = ENC_MAX_EXT_PARAMS;
    xma_enc_props->params = (XmaParameter *)calloc(1, 
                            xma_enc_props->param_cnt * sizeof(XmaParameter));

    xma_enc_props->format = XMA_VCU_NV12_FMT_TYPE;
    xma_enc_props->bits_per_pixel  = 8;
    xma_enc_props->width = enc_props->width;
    xma_enc_props->height = enc_props->height;
    xma_enc_props->rc_mode =  enc_props->custom_rc;

    switch(xma_enc_props->rc_mode) {
        case 0 : 
            break;

        case 1 : 
            if (enc_props->lookahead_depth < ENC_MIN_LOOKAHEAD_DEPTH ||
                    enc_props->lookahead_depth > ENC_MAX_LOOKAHEAD_DEPTH) {
                xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                        "Error: Provided LA Depth %d is invalid !\n", 
                        enc_props->lookahead_depth);
                xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                    "If RC mode is 1, the LA depth must lie between %d - %d\n",
                    ENC_MIN_LOOKAHEAD_DEPTH, ENC_MAX_LOOKAHEAD_DEPTH);
                return ENC_APP_FAILURE;
            } else {
                xma_enc_props->lookahead_depth = enc_props->lookahead_depth;
            }
            xma_logmsg(XMA_INFO_LOG, XLNX_ENC_APP_MODULE, 
                    "Encoder custom RC mode is enabled with LA depth = %d \n", 
                    xma_enc_props->lookahead_depth);
            break;

        default: 
            xma_enc_props->rc_mode = 0;
            xma_logmsg(XMA_INFO_LOG, XLNX_ENC_APP_MODULE, 
                    "Rate control mode is default\n");
            break;
    }

    xma_enc_props->framerate.numerator   = enc_props->fps;
    xma_enc_props->framerate.denominator = 1;

    /* Update encoder options */
    const char* RateCtrlMode = "CONST_QP";
    switch (enc_props->control_rate) {
        case 0: RateCtrlMode = "CONST_QP"; break;
        case 1: RateCtrlMode = "CBR"; break;
        case 2: RateCtrlMode = "VBR"; break;
        case 3: RateCtrlMode = "LOW_LATENCY"; break;
    }

    char FrameRate[16];
    int32_t fps_den = 1;
    sprintf(FrameRate, "%d/%d", enc_props->fps, fps_den);

    char SliceQP[8];
    if (enc_props->slice_qp == -1)
        strcpy (SliceQP, "AUTO");
    else
        sprintf(SliceQP, "%d", enc_props->slice_qp);

    const char* GopCtrlMode = "DEFAULT_GOP";
    switch (enc_props->gop_mode) {
        case 0: GopCtrlMode = "DEFAULT_GOP"; break;
        case 1: GopCtrlMode = "PYRAMIDAL_GOP"; break;
        case 2: GopCtrlMode = "LOW_DELAY_P"; break;
        case 3: GopCtrlMode = "LOW_DELAY_B"; break;
    }

    const char* GDRMode = "DISABLE";
    switch (enc_props->gdr_mode) {
        case 0: GDRMode = "DISABLE"; break;
        case 1: GDRMode = "GDR_VERTICAL"; break;
        case 2: GDRMode = "GDR_HORIZONTAL"; break;
    }

    const char* Profile = "AVC_BASELINE";
    if(enc_props->codec_id == ENCODER_ID_H264) {
        switch (enc_props->profile) {
            case ENC_H264_BASELINE: Profile = "AVC_BASELINE"; break;
            case ENC_H264_MAIN: Profile = "AVC_MAIN"; break;
            case ENC_H264_HIGH: Profile = "AVC_HIGH"; break;
            default:
                xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                    "Invalid H264 codec profile value %d \n", 
                    enc_props->profile);
                return ENC_APP_FAILURE;

        }
    } else if(enc_props->codec_id == ENCODER_ID_HEVC){
        Profile = "HEVC_MAIN";
        switch (enc_props->profile) {
            case ENC_HEVC_MAIN: Profile = "HEVC_MAIN"; break;
            case ENC_HEVC_MAIN_INTRA: Profile = "HEVC_MAIN_INTRA"; break;
            default:
                xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                    "Invalid HEVC codec profile value %d \n", 
                    enc_props->profile);
                return ENC_APP_FAILURE;
        }
    }

    const char* Level = "1";
    uint8_t is_level_found = 1;
    switch (enc_props->level) {
        case 10: Level = "1"; break;
        case 20: Level = "2"; break;
        case 21: Level = "2.1"; break;
        case 30: Level = "3"; break;
        case 31: Level = "3.1"; break;
        case 40: Level = "4"; break;
        case 41: Level = "4.1"; break;
        case 50: Level = "5"; break;
        case 51: Level = "5.1"; break;
        default:
            is_level_found = 0;
    }
    if (!is_level_found) {
        if(enc_props->codec_id == ENCODER_ID_H264) {
            switch (enc_props->level) {
                case 11: Level = "1.1"; break;
                case 12: Level = "1.2"; break;
                case 13: Level = "1.3"; break;
                case 22: Level = "2.2"; break;
                case 32: Level = "3.2"; break;
                case 42: Level = "4.2"; break;
                case 52: Level = "5.2"; break;
                default:
                    xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                            "Invalid H264 codec level value %d \n",
                            enc_props->level);
                    return ENC_APP_FAILURE;
            }
        } else if(enc_props->codec_id == ENCODER_ID_HEVC) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                       "Invalid HEVC codec level value %d \n",
                       enc_props->level);
            return ENC_APP_FAILURE;
        }
    }

    const char* Tier = "MAIN_TIER";
    switch (enc_props->tier) {
        case 0: Tier = "MAIN_TIER"; break;
        case 1: Tier = "HIGH_TIER"; break;
    }

    const char* QPCtrlMode = "UNIFORM_QP";
    switch (enc_props->qp_mode) {
        case 0: QPCtrlMode = "UNIFORM_QP"; break;
        case 1: QPCtrlMode = "AUTO_QP"; break;
        case 2: QPCtrlMode = "LOAD_QP | RELATIVE_QP"; break;
    }

    const char* DependentSlice = "FALSE";
    switch (enc_props->dependent_slice) {
        case 0: DependentSlice = "FALSE"; break;
        case 1: DependentSlice = "TRUE"; break;
    }

    const char* FillerData = "ENABLE";
    switch (enc_props->filler_data) {
        case 0: FillerData = "DISABLE"; break;
        case 1: FillerData = "ENABLE"; break;
    }

    const char* AspectRatio = "ASPECT_RATIO_AUTO";
    switch (enc_props->aspect_ratio) {
        case 0: AspectRatio = "ASPECT_RATIO_AUTO"; break;
        case 1: AspectRatio = "ASPECT_RATIO_4_3"; break;
        case 2: AspectRatio = "ASPECT_RATIO_16_9"; break;
        case 3: AspectRatio = "ASPECT_RATIO_NONE"; break;
    }

    const char* ColorSpace = "COLOUR_DESC_UNSPECIFIED";

    const char* ScalingList = "FLAT";
    switch (enc_props->scaling_list) {
        case 0: ScalingList = "FLAT"; break;
        case 1: ScalingList = "DEFAULT"; break;
    }

    const char* LoopFilter = "ENABLE";
    switch (enc_props->loop_filter) {
        case 0: LoopFilter = "DISABLE"; break;
        case 1: LoopFilter = "ENABLE"; break;
    }

    const char* EntropyMode = "MODE_CABAC";
    switch (enc_props->entropy_mode) {
        case 0: EntropyMode = "MODE_CAVLC"; break;
        case 1: EntropyMode = "MODE_CABAC"; break;
    }

    const char* ConstIntraPred = "ENABLE";
    switch (enc_props->constrained_intra_pred) {
        case 0: ConstIntraPred = "DISABLE"; break;
        case 1: ConstIntraPred = "ENABLE"; break;
    }

    const char* LambdaCtrlMode = "DEFAULT_LDA";

    const char* PrefetchBuffer = "DISABLE";
    switch (enc_props->prefetch_buffer) {
        case 0: PrefetchBuffer = "DISABLE"; break;
        case 1: PrefetchBuffer = "ENABLE"; break;
    }

    if (enc_props->tune_metrics){
        ScalingList = "FLAT";
        QPCtrlMode = "UNIFORM_QP";
    }

    if(enc_props->codec_id == ENCODER_ID_HEVC) {
        sprintf (enc_props->enc_options, "[INPUT]\n"
                "Width = %d\n"
                "Height = %d\n"
                "[RATE_CONTROL]\n"
                "RateCtrlMode = %s\n"
                "FrameRate = %s\n"
                "BitRate = %ld\n"
                "MaxBitRate = %ld\n"
                "SliceQP = %s\n"
                "MaxQP = %d\n"
                "MinQP = %d\n"
                "CPBSize = %f\n"
                "InitialDelay = %f\n"
                "[GOP]\n"
                "GopCtrlMode = %s\n"
                "Gop.GdrMode = %s\n"
                "Gop.Length = %d\n"
                "Gop.NumB = %d\n"
                "Gop.FreqIDR = %d\n"
                "[SETTINGS]\n"
                "Profile = %s\n"
                "Level = %s\n"
                "Tier = %s\n"
                "ChromaMode = CHROMA_4_2_0\n"
                "BitDepth = 8\n"
                "NumSlices = %d\n"
                "QPCtrlMode = %s\n"
                "SliceSize = %d\n"
                "DependentSlice = %s\n"
                "EnableFillerData = %s\n"
                "AspectRatio = %s\n"
                "ColourDescription = %s\n"
                "ScalingList = %s\n"
                "LoopFilter = %s\n"
                "ConstrainedIntraPred = %s\n"
                "LambdaCtrlMode = %s\n"
                "CacheLevel2 = %s\n"
                "NumCore = %d\n",
            enc_props->width, enc_props->height, RateCtrlMode, FrameRate, 
            enc_props->bit_rate, enc_props->max_bitrate, SliceQP, 
            enc_props->max_qp, enc_props->min_qp, enc_props->cpb_size,
            enc_props->initial_delay, GopCtrlMode, GDRMode, 
            enc_props->gop_size, enc_props->num_bframes, 
            enc_props->idr_period, Profile, Level, Tier, 
            enc_props->num_slices, QPCtrlMode, enc_props->slice_size, 
            DependentSlice, FillerData, AspectRatio, ColorSpace, 
            ScalingList, LoopFilter, ConstIntraPred, LambdaCtrlMode, 
            PrefetchBuffer, enc_props->num_cores);
    }
    else {
        sprintf (enc_props->enc_options, "[INPUT]\n"
                "Width = %d\n"
                "Height = %d\n"
                "[RATE_CONTROL]\n"
                "RateCtrlMode = %s\n"
                "FrameRate = %s\n"
                "BitRate = %ld\n"
                "MaxBitRate = %ld\n"
                "SliceQP = %s\n"
                "MaxQP = %d\n"
                "MinQP = %d\n"
                "CPBSize = %f\n"
                "InitialDelay = %f\n"
                "[GOP]\n"
                "GopCtrlMode = %s\n"
                "Gop.GdrMode = %s\n"
                "Gop.Length = %d\n"
                "Gop.NumB = %d\n"
                "Gop.FreqIDR = %d\n"
                "[SETTINGS]\n"
                "Profile = %s\n"
                "Level = %s\n"
                "ChromaMode = CHROMA_4_2_0\n"
                "BitDepth = 8\n"
                "NumSlices = %d\n"
                "QPCtrlMode = %s\n"
                "SliceSize = %d\n"
                "EnableFillerData = %s\n"
                "AspectRatio = %s\n"
                "ColourDescription = %s\n"
                "ScalingList = %s\n"
                "EntropyMode = %s\n"
                "LoopFilter = %s\n"
                "ConstrainedIntraPred = %s\n"
                "LambdaCtrlMode = %s\n"
                "CacheLevel2 = %s\n"
                "NumCore = %d\n",
            enc_props->width, enc_props->height, RateCtrlMode, FrameRate, 
            enc_props->bit_rate, enc_props->max_bitrate, SliceQP, 
            enc_props->max_qp, enc_props->min_qp, enc_props->cpb_size,
            enc_props->initial_delay, GopCtrlMode, GDRMode, 
            enc_props->gop_size, enc_props->num_bframes, 
            enc_props->idr_period, Profile, Level, enc_props->num_slices,
            QPCtrlMode, enc_props->slice_size, FillerData, AspectRatio, 
            ColorSpace, ScalingList, EntropyMode, LoopFilter, 
            ConstIntraPred, LambdaCtrlMode, PrefetchBuffer, 
            enc_props->num_cores);
    }

    xlnx_enc_xma_params_update(enc_props, xma_enc_props);

    return ENC_APP_SUCCESS;
}

void xlnx_la_get_xma_props(XlnxLookaheadProperties *la_props, 
                           XmaFilterProperties *xma_la_props)
{

    XmaFilterPortProperties *in_props;
    XmaFilterPortProperties *out_props;

    /* Setup lookahead properties */
    memset(xma_la_props, 0, sizeof(XmaFilterProperties));
    xma_la_props->hwfilter_type = XMA_2D_FILTER_TYPE;
    strcpy(xma_la_props->hwvendor_string, "Xilinx");

    /* Setup lookahead input port properties */
    in_props = &xma_la_props->input;
    memset(in_props, 0, sizeof(XmaFilterPortProperties));
    in_props->format = la_props->xma_fmt_type;
    in_props->bits_per_pixel = la_props->bits_per_pixel;
    in_props->width = la_props->width;
    in_props->height = la_props->height;
    in_props->stride = la_props->stride;
    in_props->framerate.numerator = la_props->framerate.numerator;
    in_props->framerate.denominator = la_props->framerate.denominator;

    /* Setup lookahead output port properties */
    out_props = &xma_la_props->output;
    memset(out_props, 0, sizeof(XmaFilterPortProperties));
    out_props->format = la_props->xma_fmt_type;
    out_props->bits_per_pixel = la_props->bits_per_pixel;
    out_props->width = 
                   XLNX_ENC_LINE_ALIGN((in_props->width), 64) >> XLNX_SCLEVEL1;
    out_props->height = 
                  XLNX_ENC_LINE_ALIGN((in_props->height), 64) >> XLNX_SCLEVEL1;
    out_props->framerate.numerator = la_props->framerate.numerator;
    out_props->framerate.denominator = la_props->framerate.denominator;

    xlnx_la_xma_params_update(la_props, xma_la_props);
    return;
}

void xlnx_enc_free_xma_props(XmaEncoderProperties *xma_enc_props)
{
    if(xma_enc_props->params)
        free(xma_enc_props->params);

    return;
}

void xlnx_la_free_xma_props(XmaFilterProperties *xma_la_props)
{
    if(xma_la_props->params)
        free(xma_la_props->params);

    return;
}

static int32_t xlnx_enc_fill_pool_props(xrmCuPoolProperty *enc_cu_pool_prop, 
                                        XlnxEncoderXrmCtx *enc_xrm_ctx)
{
    int32_t cu_num = 0;
    enc_cu_pool_prop->cuListProp.sameDevice = true;
    enc_cu_pool_prop->cuListNum = 1;

    if (enc_xrm_ctx->enc_load > 0) {
        strcpy(enc_cu_pool_prop->cuListProp.cuProps[cu_num].kernelName, 
               "encoder");
        strcpy(enc_cu_pool_prop->cuListProp.cuProps[cu_num].kernelAlias, 
               "ENCODER_MPSOC");
        enc_cu_pool_prop->cuListProp.cuProps[cu_num].devExcl = false;
        enc_cu_pool_prop->cuListProp.cuProps[cu_num].requestLoad = 
                                                     XRM_PRECISION_1000000_BIT_MASK(enc_xrm_ctx->enc_load);
        cu_num++;

        for(int32_t i = 0; i < enc_xrm_ctx->enc_num; i++) {
            strcpy(enc_cu_pool_prop->cuListProp.cuProps[cu_num].kernelName, 
                   "kernel_vcu_encoder");
            strcpy(enc_cu_pool_prop->cuListProp.cuProps[cu_num].kernelAlias, 
                   "");
            enc_cu_pool_prop->cuListProp.cuProps[cu_num].devExcl = false;
            enc_cu_pool_prop->cuListProp.cuProps[cu_num].requestLoad = XRM_PRECISION_1000000_BIT_MASK(XRM_MAX_CU_LOAD_GRANULARITY_1000000);
            cu_num++;
        }
    }

    if(enc_xrm_ctx->la_load > 0) {
        strcpy(enc_cu_pool_prop->cuListProp.cuProps[cu_num].kernelName, 
                "lookahead");
        strcpy(enc_cu_pool_prop->cuListProp.cuProps[cu_num].kernelAlias, 
                "LOOKAHEAD_MPSOC");
        enc_cu_pool_prop->cuListProp.cuProps[cu_num].devExcl = false;
        enc_cu_pool_prop->cuListProp.cuProps[cu_num].requestLoad = 
                                                     XRM_PRECISION_1000000_BIT_MASK(enc_xrm_ctx->la_load);
        cu_num++;

    }

    enc_cu_pool_prop->cuListProp.cuNum = cu_num;
    return ENC_APP_SUCCESS;

}

static int32_t xlnx_la_load_calc(XlnxEncoderXrmCtx *enc_xrm_ctx, 
                                 XmaEncoderProperties *xma_enc_props)
{

    int32_t la_load = 0;
    int32_t skip_value = 0;
    int32_t func_id = 0;
    char pluginName[XRM_MAX_NAME_LEN];
    XmaFilterProperties filter_props;
    xrmPluginFuncParam plg_param;

    /* Update the lookahead props that are needed for libxmaPropsTOjson */
    filter_props.input.width = xma_enc_props->width;
    filter_props.input.height = xma_enc_props->height;
    filter_props.input.framerate.numerator = xma_enc_props->framerate.numerator;
    filter_props.input.framerate.denominator = 
                                 xma_enc_props->framerate.denominator;

    memset(&plg_param, 0, sizeof(xrmPluginFuncParam));
    void *handle;
    void (*convertXmaPropsToJson)(void* props, char* funcName, char* jsonJob);

    /* Loading propstojson library to convert LA properties to json */
    handle = dlopen("/opt/xilinx/xrm/plugin/libxmaPropsTOjson.so", RTLD_NOW );

    convertXmaPropsToJson = dlsym(handle, "convertXmaPropsToJson");
    (*convertXmaPropsToJson) (&filter_props, "LOOKAHEAD", plg_param.input);
    dlclose(handle);

    strcpy(pluginName, "xrmU30EncPlugin");

    if (xrmExecPluginFunc(enc_xrm_ctx->xrm_ctx, pluginName, func_id, &plg_param) 
                          != XRM_SUCCESS) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                   "XRM LA plugin failed \n");
        return ENC_APP_FAILURE;
    }
    else
    {
        skip_value = atoi((char *)(strtok(plg_param.output, " ")));
        skip_value = atoi((char *)(strtok(NULL, " ")));
        /* To silence the warning of skip_value set, but not used */
        (void)skip_value;
        la_load = atoi((char *)(strtok(NULL, " ")));
    }
    return la_load;

}

static int32_t xlnx_enc_load_calc(XlnxEncoderXrmCtx *enc_xrm_ctx,
                                  XmaEncoderProperties *xma_enc_props, 
                                  int32_t lookahead_enable,
                                  xrmCuPoolProperty *enc_cu_pool_prop)
{

    int32_t func_id = 0;
    char pluginName[XRM_MAX_NAME_LEN];
    xrmPluginFuncParam plg_param;

    memset(&plg_param, 0, sizeof(xrmPluginFuncParam));
    void *handle;
    void (*convertXmaPropsToJson)(void* props, char* funcName, char* jsonJob);

    /* Loading propstojson library to convert encoder properties to json */
    handle = dlopen("/opt/xilinx/xrm/plugin/libxmaPropsTOjson.so", RTLD_NOW );

    convertXmaPropsToJson = dlsym(handle, "convertXmaPropsToJson");
    (*convertXmaPropsToJson) (xma_enc_props, "ENCODER", plg_param.input);
    dlclose(handle);

    strcpy(pluginName, "xrmU30EncPlugin");

    if (xrmExecPluginFunc(enc_xrm_ctx->xrm_ctx, pluginName, func_id, 
                          &plg_param) != XRM_SUCCESS) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                "XRM encoder plugin failed \n");
        return ENC_APP_FAILURE;
    }
    else {
        enc_xrm_ctx->enc_load = atoi((char*)(strtok(plg_param.output, " ")));
        enc_xrm_ctx->enc_num = atoi((char*)(strtok(NULL, " ")));
    }

    /* If LA is enabled, calculate the load to reserve the CU*/
    if(lookahead_enable) {
        enc_xrm_ctx->la_load = xlnx_la_load_calc(enc_xrm_ctx, xma_enc_props);
        if(enc_xrm_ctx->la_load <= 0) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                    "Lookahead XRM load calculation failed \n");
            return ENC_APP_FAILURE;
        }
    }
    xlnx_enc_fill_pool_props(enc_cu_pool_prop, enc_xrm_ctx);

    return ENC_APP_SUCCESS;

}

int32_t xlnx_enc_device_init(XlnxEncoderXrmCtx *enc_xrm_ctx,XmaEncoderProperties *xma_enc_props,int32_t lookahead_enable)
{

    xrmCuPoolProperty enc_cu_pool_prop;
    int32_t ret = ENC_APP_FAILURE;
    int32_t num_cu_pool = 0;

    memset(&enc_cu_pool_prop, 0, sizeof(enc_cu_pool_prop));

    /* Create XRM context */
    enc_xrm_ctx->xrm_ctx = (xrmContext *)xrmCreateContext(XRM_API_VERSION_1);
    if (enc_xrm_ctx->xrm_ctx == NULL) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                "create local XRM context failed\n");
        return ret;
    }

    /* Calculate encoder load based on encoder properties */
    ret = xlnx_enc_load_calc(enc_xrm_ctx, xma_enc_props, lookahead_enable, 
                             &enc_cu_pool_prop);
    if(ret != ENC_APP_SUCCESS) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                "Enc load calculation failed %d \n", ret);
        return ret;
    }

    /* Check the number of pools available for the given encoder load */
    num_cu_pool = xrmCheckCuPoolAvailableNum(enc_xrm_ctx->xrm_ctx, 
            &enc_cu_pool_prop);
    if(num_cu_pool <= 0) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                "No resources available for allocation \n");
        return ENC_APP_FAILURE;
    }

    /* If the device reservation ID is not sent through command line, get the
       next available device id */
    if(enc_xrm_ctx->device_id < 0) {

        /* Query XRM to get reservation index for the required CU */
        enc_xrm_ctx->enc_res_idx = xrmCuPoolReserve(enc_xrm_ctx->xrm_ctx, 
                &enc_cu_pool_prop);
        if (enc_xrm_ctx->enc_res_idx == 0) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                    "Failed to reserve encode cu pool\n");
            return ENC_APP_FAILURE;
        }
    }

    return ENC_APP_SUCCESS;

}

void xlnx_enc_xrm_deinit(XlnxEncoderXrmCtx *enc_xrm_ctx)
{

    if(enc_xrm_ctx->enc_res_in_use) {
        xrmCuListRelease(enc_xrm_ctx->xrm_ctx, 
                         &enc_xrm_ctx->encode_cu_list_res);
    }

    if((enc_xrm_ctx->device_id < 0) && (enc_xrm_ctx->enc_res_idx >= 0)) {
        xrmCuPoolRelinquish(enc_xrm_ctx->xrm_ctx, enc_xrm_ctx->enc_res_idx);
    }

    if (enc_xrm_ctx->lookahead_res_inuse ==1) {
        xrmCuRelease(enc_xrm_ctx->xrm_ctx, &enc_xrm_ctx->lookahead_cu_res);
    }

    xrmDestroyContext(enc_xrm_ctx->xrm_ctx);
    return;
}

static void xlnx_enc_app_close(XlnxEncoderCtx *enc_ctx, 
                          XmaEncoderProperties *xma_enc_props,
                          XmaFilterProperties  *xma_la_props)
{
    xlnx_enc_deinit(enc_ctx, xma_enc_props);
    xlnx_la_deinit(&enc_ctx->la_ctx, xma_la_props);
    xlnx_enc_xrm_deinit(&enc_ctx->enc_xrm_ctx);

    return;
}

static int32_t xlnx_la_get_num_planes(XmaFormatType format)
{
    /* multi scaler supports max 2 planes till v2019.1 */
    switch (format) {
        case XMA_RGB888_FMT_TYPE: /* BGR */
            return 1;
        case XMA_YUV420_FMT_TYPE: /* NV12 */
            return 2;
        case XMA_VCU_NV12_FMT_TYPE: /* VCU_NV12 */
            return 1;
        default:
            xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                       "Unsupported format...");
            return -1;
    }
}

static int32_t xlnx_la_allocate_xrm_cu(XlnxLookaheadCtx *la_ctx, XlnxEncoderXrmCtx *enc_xrm_ctx,XmaFilterProperties *xma_la_props)
{

    int32_t ret = ENC_APP_FAILURE;
    /* XRM lookahead allocation */
    xrmCuProperty lookahead_cu_prop;

    memset(&lookahead_cu_prop, 0, sizeof(xrmCuProperty));
    memset(&enc_xrm_ctx->lookahead_cu_res, 0, sizeof(xrmCuResource));

    strcpy(lookahead_cu_prop.kernelName, "lookahead");
    strcpy(lookahead_cu_prop.kernelAlias, "LOOKAHEAD_MPSOC");
    lookahead_cu_prop.devExcl = false;
    lookahead_cu_prop.requestLoad = XRM_PRECISION_1000000_BIT_MASK(enc_xrm_ctx->la_load);

    if(enc_xrm_ctx->device_id < 0) {
        lookahead_cu_prop.poolId = enc_xrm_ctx->enc_res_idx;
        ret = xrmCuAlloc(enc_xrm_ctx->xrm_ctx, &lookahead_cu_prop,
                         &enc_xrm_ctx->lookahead_cu_res);
    }
    else {
        ret = xrmCuAllocFromDev(enc_xrm_ctx->xrm_ctx,  enc_xrm_ctx->device_id,
                          &lookahead_cu_prop, &enc_xrm_ctx->lookahead_cu_res);
    }

    if (ret != 0) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                "xrm_allocation: fail to allocate lookahead cu \n");
        return ret;
    } else {
        enc_xrm_ctx->lookahead_res_inuse = 1;
    }

    /* Set XMA plugin SO and device index */
    xma_la_props->plugin_lib = 
                         enc_xrm_ctx->lookahead_cu_res.kernelPluginFileName;
    xma_la_props->dev_index = enc_xrm_ctx->lookahead_cu_res.deviceId;
    /* XMA to select the ddr bank based on xclbin meta data */
    xma_la_props->ddr_bank_index = -1;
    xma_la_props->cu_index = enc_xrm_ctx->lookahead_cu_res.cuId;
    xma_la_props->channel_id = enc_xrm_ctx->lookahead_cu_res.channelId;

    return ret;
}

int32_t xlnx_la_create(XlnxLookaheadCtx *la_ctx, XlnxEncoderXrmCtx *enc_xrm_ctx, XmaFilterProperties *xma_la_props)
{

    XlnxLookaheadProperties *la_props = &la_ctx->la_props;

    if (!la_ctx) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                "No LA context received\n");
        return ENC_APP_FAILURE;
    }
    if ((la_props->lookahead_depth == 0) && 
        (la_props->temporal_aq_mode == 1)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                "Invalid params: Lookahead = 0, temporal aq=%u\n",
                la_props->temporal_aq_mode);
        return ENC_APP_SUCCESS;
    }

    if (((la_props->lookahead_depth == 0) && (la_props->spatial_aq_mode == 0))
    || ((la_props->spatial_aq_mode == 0) && (la_props->temporal_aq_mode == 0) 
    && (la_props->rate_control_mode == 0))) {
        la_ctx->bypass = 1;
        return ENC_APP_SUCCESS;
    }

    la_ctx->num_planes = xlnx_la_get_num_planes(la_ctx->la_props.xma_fmt_type);
    la_ctx->bypass = 0;

    xlnx_la_get_xma_props(&la_ctx->la_props, xma_la_props);

    enc_xrm_ctx->lookahead_res_inuse = 0;
    xlnx_la_allocate_xrm_cu(la_ctx, enc_xrm_ctx, xma_la_props);

    /* Create lookahead session based on the requested properties */
    la_ctx->filter_session = xma_filter_session_create(xma_la_props);
    if (!la_ctx->filter_session) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                "Failed to create lookahead session\n");
        return ENC_APP_FAILURE;
    }

    la_ctx->xma_la_frame = (XmaFrame *) calloc(1, sizeof(XmaFrame));
    if (la_ctx->xma_la_frame == NULL) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                "Out of memory while allocating la out frame. \n");
        return ENC_APP_FAILURE;
    }

    return ENC_APP_SUCCESS;
}

int32_t xlnx_la_get_bypass_mode(XlnxLookaheadCtx *la_ctx)
{
    if (!la_ctx) {
        return ENC_APP_FAILURE;
    }
    return la_ctx->bypass;
}

int32_t xlnx_enc_la_init(XlnxEncoderCtx *enc_ctx, 
                         XmaFilterProperties  *xma_la_props)
{
    int32_t ret = ENC_APP_FAILURE;
    XlnxLookaheadProperties *la_props = &enc_ctx->la_ctx.la_props;
    XlnxEncoderProperties *enc_props = &enc_ctx->enc_props;

    la_props->width = enc_props->width;
    la_props->height = enc_props->height;
    la_props->framerate.numerator = enc_props->fps;
    la_props->framerate.denominator = 1;

    //TODO: Assume 256 aligned for now. Needs to be fixed later
    la_props->stride = XLNX_ENC_LINE_ALIGN(enc_props->width, VCU_STRIDE_ALIGN);
    la_props->bits_per_pixel = 8;

    if (enc_props->gop_size <= 0) {
        la_props->gop_size = ENC_DEFAULT_GOP_SIZE;
    } else {
        la_props->gop_size = enc_props->gop_size;
    }

    la_props->lookahead_depth = enc_props->lookahead_depth;
    la_props->spatial_aq_mode = enc_props->spatial_aq;
    la_props->spatial_aq_gain = enc_props->spatial_aq_gain;
    la_props->temporal_aq_mode = enc_props->temporal_aq;
    la_props->rate_control_mode = enc_props->custom_rc;
    la_props->num_bframes = enc_props->num_bframes;
    la_props->latency_logging = enc_props->latency_logging;

    /* Only NV12 format is supported in this application */
    la_props->xma_fmt_type = XMA_VCU_NV12_FMT_TYPE;
    la_props->enable_hw_buf = 0;

    switch (enc_props->codec_id) {
        case ENCODER_ID_H264:
            la_props->codec_type = LOOKAHEAD_ID_H264;
            break;
        case ENCODER_ID_HEVC:
            la_props->codec_type = LOOKAHEAD_ID_HEVC;
            break;
    }

    ret = xlnx_la_create(&enc_ctx->la_ctx, &enc_ctx->enc_xrm_ctx, xma_la_props);
    if (ret != ENC_APP_SUCCESS) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                "Error : init_la : create_xlnx_la Failed \n");
        return ENC_APP_FAILURE;
    }

    enc_ctx->la_bypass = xlnx_la_get_bypass_mode(&enc_ctx->la_ctx);

    return ENC_APP_SUCCESS;
}

static void xlnx_enc_context_init(XlnxEncoderCtx *enc_ctx)
{

    XlnxEncoderProperties *enc_props = &enc_ctx->enc_props;

    /* Initialize the encoder parameters to default */
    enc_ctx->enc_xrm_ctx.device_id = -1;
    enc_ctx->enc_xrm_ctx.enc_res_idx = -1;
    enc_ctx->enc_xrm_ctx.enc_res_in_use = 0;
    enc_ctx->enc_xrm_ctx.lookahead_res_inuse = 0;

    enc_ctx->loop_count = 0;
    enc_ctx->num_frames = SIZE_MAX;
    enc_props->codec_id = -1;
    enc_props->width = ENC_DEFAULT_WIDTH;
    enc_props->height = ENC_DEFAULT_HEIGHT;
    enc_props->bit_rate = ENC_DEFAULT_BITRATE;
    enc_props->fps = ENC_DEFAULT_FRAMERATE;
    enc_props->gop_size = ENC_DEFAULT_GOP_SIZE;
    enc_props->slice_qp = -1;
    enc_props->control_rate = 1;
    enc_props->custom_rc = 0;
    enc_props->max_bitrate = ENC_DEFAULT_MAX_BITRATE;
    enc_props->min_qp = 0;
    enc_props->max_qp = ENC_SUPPORTED_MAX_QP;
    enc_props->cpb_size = 2.0;
    enc_props->initial_delay = 1.0;
    enc_props->gop_mode = 0;
    enc_props->gdr_mode = 0;
    enc_props->num_bframes = ENC_DEFAULT_NUM_B_FRAMES;
    enc_props->idr_period = -1;

    /* Assigning the default profile as HEVC profile. If the codec option 
       is H264, this will be updated */
    enc_props->profile = ENC_HEVC_MAIN;
    enc_props->level = ENC_DEFAULT_LEVEL;
    enc_props->tier = 0;
    enc_props->num_slices = 1;
    enc_props->qp_mode = 1;
    enc_props->aspect_ratio = 0;
    enc_props->lookahead_depth = 0;
    enc_props->temporal_aq = 1;
    enc_props->spatial_aq = 1;
    enc_props->spatial_aq_gain = ENC_DEFAULT_SPAT_AQ_GAIN;
    enc_props->scaling_list = 1;
    enc_props->filler_data = 0;
    enc_props->dependent_slice = 0;
    enc_props->slice_size = 0;
    enc_props->entropy_mode = 1;
    enc_props->loop_filter = 1;
    enc_props->constrained_intra_pred = 0;
    enc_props->prefetch_buffer = 1;
    enc_props->latency_logging = 0;
    enc_props->enable_hw_buf = 1;
    enc_props->num_cores = 0;
    enc_props->tune_metrics = 0;

    enc_ctx->pts = 0;
    enc_ctx->out_frame_cnt = 0;
    enc_ctx->in_frame_cnt = 0;
    enc_ctx->enc_state = ENC_READ_INPUT;
    enc_ctx->la_in_frame = &(enc_ctx->in_frame);
    enc_ctx->enc_in_frame = &(enc_ctx->in_frame);

}

static int32_t xlnx_enc_update_props(XlnxEncoderCtx *enc_ctx, 
                                     XmaEncoderProperties *xma_enc_props)
{

    XlnxEncoderProperties *enc_props = &enc_ctx->enc_props;
    enc_props->enc_options = calloc(1, ENC_MAX_OPTIONS_SIZE);

    /* Enable custom rate control when rate control is set to CBR and 
    lookahead is set, disable when expert option lookahead-rc-off is set. */
    if((enc_props->control_rate == 1) && (enc_props->lookahead_depth > 1)) {
        enc_props->custom_rc = 1;
    }

    /* Enable Adaptive Quantization by default, if lookahead is enabled */
    if (enc_props->lookahead_depth >= 1 && (enc_props->temporal_aq == 1 || 
                enc_props->spatial_aq == 1) && (enc_props->tune_metrics == 0)) {
        xma_logmsg(XMA_INFO_LOG, XLNX_ENC_APP_MODULE, 
                "Setting qp mode to 2, as the lookahead params are set \n");
        enc_props->qp_mode = 2;
    }
    else if ((enc_props->lookahead_depth == 0) || 
            (enc_props->tune_metrics == 1)) {
        if (enc_props->temporal_aq)
            enc_props->temporal_aq = 0;

        if (enc_props->spatial_aq)
            enc_props->spatial_aq = 0;
    }

    /* Tunes video quality for objective scores by setting flat scaling-list 
       and uniform qp-mode */
    if (enc_props->tune_metrics){
        enc_props->scaling_list = 0;
        enc_props->qp_mode = 0;
    }

    /* Enable Adaptive Quantization by default, if lookahead is enabled */
    if (enc_props->lookahead_depth >= 1 && (enc_props->temporal_aq == 1 || 
        enc_props->spatial_aq == 1)) {
        xma_logmsg(XMA_INFO_LOG, XLNX_ENC_APP_MODULE, 
                "Setting qp mode to 2, as the lookahead params are set \n");
        enc_props->qp_mode = 2;
    }
    else if (enc_props->lookahead_depth == 0) {
        if (enc_props->temporal_aq)
            enc_props->temporal_aq = 0;

        if (enc_props->spatial_aq)
            enc_props->spatial_aq = 0;

        enc_props->enable_hw_buf = 0;
    }

    /* Set IDR period to gop-size, when the user has not specified it on 
       the command line */
    if (enc_props->idr_period == -1)
    {
        if (enc_props->gop_size > 0){
            enc_props->idr_period = enc_props->gop_size;
        }
        xma_logmsg(XMA_INFO_LOG, XLNX_ENC_APP_MODULE, 
                "Setting IDR period to GOP size \n");
    }

    return xlnx_enc_get_xma_props(enc_props, xma_enc_props);
}

static void xlnx_enc_frame_init(XlnxEncoderCtx *enc_ctx)
{

    XmaFrameProperties *frame_props = &(enc_ctx->in_frame.frame_props);
    frame_props->format = XMA_VCU_NV12_FMT_TYPE;
    frame_props->width  = enc_ctx->enc_props.width;
    frame_props->height = enc_ctx->enc_props.height;
    frame_props->linesize[0] = enc_ctx->enc_props.width;
    frame_props->linesize[1] = enc_ctx->enc_props.width;
    frame_props->bits_per_pixel = 8;

    return;
}

static int32_t xlnx_xlnx_enc_cu_alloc_device_id(XlnxEncoderXrmCtx *enc_xrm_ctx, 
                                        XmaEncoderProperties *xma_enc_props)
{

    xrmCuProperty encode_cu_hw_prop, encode_cu_sw_prop;

    int32_t ret = -1;

    memset(&encode_cu_hw_prop, 0, sizeof(xrmCuProperty));
    memset(&encode_cu_sw_prop, 0, sizeof(xrmCuProperty));
    memset(&enc_xrm_ctx->encode_cu_list_res, 0, sizeof(xrmCuListResource));

    strcpy(encode_cu_hw_prop.kernelName, "encoder");
    strcpy(encode_cu_hw_prop.kernelAlias, "ENCODER_MPSOC");
    encode_cu_hw_prop.devExcl = false;
    encode_cu_hw_prop.requestLoad = XRM_PRECISION_1000000_BIT_MASK(enc_xrm_ctx->enc_load);

    strcpy(encode_cu_sw_prop.kernelName, "kernel_vcu_encoder");
    encode_cu_sw_prop.devExcl = false;
    encode_cu_sw_prop.requestLoad = XRM_PRECISION_1000000_BIT_MASK(XRM_MAX_CU_LOAD_GRANULARITY_1000000);

    ret = xrmCuAllocFromDev(enc_xrm_ctx->xrm_ctx, enc_xrm_ctx->device_id, 
            &encode_cu_hw_prop, &enc_xrm_ctx->encode_cu_list_res.cuResources[0]);

    if (ret <= ENC_APP_FAILURE)
    {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                   "xrm failed to allocate encoder resources on device %d\n", 
                   enc_xrm_ctx->device_id);
        return ret;
    }
    else
    {
        ret = xrmCuAllocFromDev(enc_xrm_ctx->xrm_ctx, enc_xrm_ctx->device_id, 
                &encode_cu_sw_prop, &enc_xrm_ctx->encode_cu_list_res.cuResources[1]);
        if (ret <= ENC_APP_FAILURE)
        {
            xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                    "xrm failed to allocate encoder resources on device %d\n", 
                    enc_xrm_ctx->device_id);
            return ret;
        }
    }

    /* Set XMA plugin SO and device index */
    xma_enc_props->plugin_lib = 
        enc_xrm_ctx->encode_cu_list_res.cuResources[0].kernelPluginFileName;
    xma_enc_props->dev_index = 
        enc_xrm_ctx->encode_cu_list_res.cuResources[0].deviceId;
    /* XMA to select the ddr bank based on xclbin meta data */
    xma_enc_props->ddr_bank_index = -1;
    xma_enc_props->cu_index = 
        enc_xrm_ctx->encode_cu_list_res.cuResources[1].cuId;
    xma_enc_props->channel_id = 
        enc_xrm_ctx->encode_cu_list_res.cuResources[1].channelId;

    enc_xrm_ctx->enc_res_in_use = 1;

    return ret;
}

static int32_t xlnx_xlnx_enc_cu_alloc_reserve_id(XlnxEncoderXrmCtx *enc_xrm_ctx, 
                                        XmaEncoderProperties *xma_enc_props)
{

    int32_t ret = ENC_APP_FAILURE;

    /* XRM encoder allocation */
    xrmCuListProperty encode_cu_list_prop;

    memset(&encode_cu_list_prop, 0, sizeof(xrmCuListProperty));
    memset(&enc_xrm_ctx->encode_cu_list_res, 0, sizeof(xrmCuListResource));

    encode_cu_list_prop.cuNum = 2;
    strcpy(encode_cu_list_prop.cuProps[0].kernelName, "encoder");
    strcpy(encode_cu_list_prop.cuProps[0].kernelAlias, "ENCODER_MPSOC");
    encode_cu_list_prop.cuProps[0].devExcl = false;
    encode_cu_list_prop.cuProps[0].requestLoad = XRM_PRECISION_1000000_BIT_MASK(enc_xrm_ctx->enc_load);
    encode_cu_list_prop.cuProps[0].poolId = enc_xrm_ctx->enc_res_idx;

    strcpy(encode_cu_list_prop.cuProps[1].kernelName, "kernel_vcu_encoder");
    encode_cu_list_prop.cuProps[1].devExcl = false;
    encode_cu_list_prop.cuProps[1].requestLoad = XRM_PRECISION_1000000_BIT_MASK(XRM_MAX_CU_LOAD_GRANULARITY_1000000);
    encode_cu_list_prop.cuProps[1].poolId = enc_xrm_ctx->enc_res_idx;

    ret = xrmCuListAlloc(enc_xrm_ctx->xrm_ctx, &encode_cu_list_prop, 
            &enc_xrm_ctx->encode_cu_list_res);
    if (ret != ENC_APP_SUCCESS) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                "Failed to allocate encoder cu from reserve id \n");
        return ret;
    }

    /* Set XMA plugin SO and device index */
    xma_enc_props->plugin_lib = 
        enc_xrm_ctx->encode_cu_list_res.cuResources[0].kernelPluginFileName;
    xma_enc_props->dev_index = 
        enc_xrm_ctx->encode_cu_list_res.cuResources[0].deviceId;
    /* XMA to select the ddr bank based on xclbin meta data */
    xma_enc_props->ddr_bank_index = -1;
    xma_enc_props->cu_index = 
        enc_xrm_ctx->encode_cu_list_res.cuResources[1].cuId;
    xma_enc_props->channel_id = 
        enc_xrm_ctx->encode_cu_list_res.cuResources[1].channelId;

    enc_xrm_ctx->enc_res_in_use = 1;

    return ret;
}

static int32_t xlnx_enc_cu_alloc(XlnxEncoderXrmCtx *enc_xrm_ctx, 
                                 XmaEncoderProperties *xma_enc_props)
{

    int32_t ret = ENC_APP_FAILURE;

    if(enc_xrm_ctx->device_id >= 0) {
        ret = xlnx_xlnx_enc_cu_alloc_device_id(enc_xrm_ctx, xma_enc_props);
    }
    else {
        ret = xlnx_xlnx_enc_cu_alloc_reserve_id(enc_xrm_ctx, xma_enc_props);
    }

    return ret;
}

int32_t xlnx_enc_create_session(XlnxEncoderCtx *enc_ctx, 
                                XmaEncoderProperties *xma_enc_props)
{

    XlnxEncoderXrmCtx *enc_xrm_ctx = &enc_ctx->enc_xrm_ctx;
    if(xlnx_enc_cu_alloc(enc_xrm_ctx, xma_enc_props) != ENC_APP_SUCCESS) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                "Error in encoder CU allocation \n");
        return ENC_APP_FAILURE;
    }

    /* Encoder session creation */
    enc_ctx->enc_session = xma_enc_session_create(xma_enc_props);
    if(enc_ctx->enc_session == NULL) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                "Encoder session creation failed \n");
        return ENC_APP_FAILURE;
    }

    #ifdef U30V2
    XmaDataBuffer* output_xma_buffer = &enc_ctx->xma_buffer;
    /* Allocate enough data to safely recv */
    output_xma_buffer->alloc_size  = (3 * enc_ctx->enc_props.width *
                            enc_ctx->enc_props.height) >> 1;
    output_xma_buffer->data.buffer = malloc(output_xma_buffer->alloc_size);
    if(!output_xma_buffer->data.buffer) {
         xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                    "Encoder failed to allocate data buffer for recv! \n");
    }
    #endif

    return ENC_APP_SUCCESS;
}

static int32_t xlnx_la_send_frame(XlnxLookaheadCtx *la_ctx, XmaFrame *in_frame)
{

    int32_t rc;
    if (!la_ctx) 
	{
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, "xlnx_la_send_frame : XMA_ERROR\n");
        return ENC_APP_FAILURE;
    }
    if (in_frame && in_frame->do_not_encode) 
	{
        rc = ENC_APP_SUCCESS;
    } 
	else 
	{
        rc = xma_filter_session_send_frame(la_ctx->filter_session,
                in_frame);
    }
    if (rc <= XMA_ERROR) 
	{
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, "xlnx_la_send_frame : Send frame to LA xma plg Failed!!\n");
        rc = ENC_APP_FAILURE;
    }
    return rc;
}

int32_t xlnx_la_process_frame(XlnxLookaheadCtx *la_ctx, XmaFrame *in_frame,XmaFrame **out_frame)
{
    int32_t ret = 0;
    if (out_frame == NULL) 
	{
        return XMA_ERROR;
    }
    if (la_ctx->bypass == 1) 
	{
        *out_frame = in_frame;
        return XMA_SUCCESS;
    }
    if (la_ctx->xma_la_frame == NULL) 
	{
        return XMA_ERROR;
    }
    ret = xlnx_la_send_frame(la_ctx, in_frame);
    switch (ret) 
	{
        case XMA_SUCCESS:
            ret = xma_filter_session_recv_frame(la_ctx->filter_session, 
                    la_ctx->xma_la_frame);
            if (ret == XMA_TRY_AGAIN) {
                ret = XMA_SEND_MORE_DATA;
            }
            if(ret != ENC_APP_SUCCESS)
                break;
        case XMA_SEND_MORE_DATA:
            break;
        case XMA_TRY_AGAIN:
            /* If the user is receiving output, this condition should 
               not be hit */
            ret = xma_filter_session_recv_frame(la_ctx->filter_session, la_ctx->xma_la_frame);
            if (ret == XMA_SUCCESS) 
			{
                ret = xlnx_la_send_frame(la_ctx, in_frame);
            }
            break;
        case XMA_ERROR:
        default:
            *out_frame = NULL;
            break;
    }
    if (ret == XMA_SUCCESS) 
	{
        *out_frame = la_ctx->xma_la_frame;
        la_ctx->xma_la_frame = NULL;
    }
    return ret;
}

int32_t xlnx_la_release_frame(XlnxLookaheadCtx *la_ctx, XmaFrame *received_frame)
{
    if (la_ctx->bypass) 
	{
        return ENC_APP_SUCCESS;
    }
    if (!received_frame || la_ctx->xma_la_frame) 
	{
        return ENC_APP_FAILURE;
    }
    la_ctx->xma_la_frame = received_frame;
    XmaSideDataHandle *side_data = la_ctx->xma_la_frame->side_data;
    memset(la_ctx->xma_la_frame, 0, sizeof(XmaFrame));
    la_ctx->xma_la_frame->side_data = side_data;
    return ENC_APP_SUCCESS;
}

int32_t xlnx_la_deinit(XlnxLookaheadCtx *la_ctx, XmaFilterProperties *xma_la_props)
{
    if (!la_ctx) {
        return ENC_APP_FAILURE;
    }
    if (la_ctx->bypass == 0) 
	{
        if (la_ctx->filter_session) {
            xma_filter_session_destroy(la_ctx->filter_session);
            la_ctx->filter_session = NULL;
        }
        if (la_ctx->xma_la_frame != NULL) 
		{
            xma_frame_clear_all_side_data(la_ctx->xma_la_frame);
            free(la_ctx->xma_la_frame);
            la_ctx->xma_la_frame = NULL;
        }
    }
    xlnx_la_free_xma_props(xma_la_props);
    return ENC_APP_SUCCESS;
}

void xlnx_enc_deinit(XlnxEncoderCtx *enc_ctx, XmaEncoderProperties *xma_enc_props)
{
    if(enc_ctx->enc_session != NULL)
        xma_enc_session_destroy(enc_ctx->enc_session);
    free(enc_ctx->enc_props.enc_options);
    xlnx_enc_free_xma_props(xma_enc_props);

    #ifdef U30V2
    if(enc_ctx->xma_buffer.data.buffer) {
        free(enc_ctx->xma_buffer.data.buffer);
    }
    #endif
}

int loadyuv(char *ybuf, char *uvbuf, FILE *hInputYUVFile)
{
	int ylen = 1920 * 1080;
	int uvlen = 1920 * 1080 / 2; 
	
	fread(ybuf, ylen, 1, hInputYUVFile);
	fread(uvbuf, uvlen, 1, hInputYUVFile);

	return 0;
}

int Encoder_Init(XlnxEncoderCtx *enc_ctx,XmaEncoderProperties *xma_enc_props,XmaFilterProperties *xma_la_props)
{
	int32_t ret = ENC_APP_SUCCESS;
	
    memset(enc_ctx, 0, sizeof(enc_ctx));
	
    xlnx_enc_context_init(enc_ctx);
	
	enc_ctx->enc_props.width=3840;
	enc_ctx->enc_props.height=2160;
	enc_ctx->enc_props.fps = 30;
	enc_ctx->enc_props.codec_id = 1;

    if(xlnx_enc_update_props(enc_ctx, xma_enc_props) != ENC_APP_SUCCESS)
        return ENC_APP_FAILURE;
	
    xlnx_enc_frame_init(enc_ctx);

    if((ret = xlnx_enc_device_init(&enc_ctx->enc_xrm_ctx, xma_enc_props, enc_ctx->enc_props.lookahead_depth)) != ENC_APP_SUCCESS) 
	{
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                "Device Init failed with error %d \n", ret);
        xlnx_enc_app_close(enc_ctx, xma_enc_props, xma_la_props);
        return -1;
    }
    /* Lookahead session creation*/
    if((ret = xlnx_enc_la_init(enc_ctx, xma_la_props)) != ENC_APP_SUCCESS) {
        xlnx_enc_app_close(enc_ctx, xma_enc_props, xma_la_props);
        return -1;
    }
    if((ret = xlnx_enc_create_session(enc_ctx, xma_enc_props)) != 
                                                            ENC_APP_SUCCESS) {
        xlnx_enc_app_close(enc_ctx, xma_enc_props, xma_la_props);
        return -1;
    }
	printf("===============init %d %d \n",enc_ctx->enc_props.width,enc_ctx->enc_props.height);
	return ENC_APP_SUCCESS;
}

int Encoder_frame(XlnxEncoderCtx *enc_ctx,char* iyBuf,char* iuvBuf,char* outBuf,int* outlen)
{

    uint32_t ret = ENC_APP_SUCCESS;
    uint32_t frame_size_y = (enc_ctx->enc_props.width *enc_ctx->enc_props.height);
	uint32_t frame_size_uv = frame_size_y /2 ;

	XmaFrame *xma_frame = &(enc_ctx->in_frame);
    xma_frame->data[0].refcount = 1;
    xma_frame->data[0].buffer_type = XMA_HOST_BUFFER_TYPE;
    xma_frame->data[0].is_clone = false;
    xma_frame->data[0].buffer = calloc(1, frame_size_y);
    xma_frame->data[1].refcount = 1;
    xma_frame->data[1].buffer_type = XMA_HOST_BUFFER_TYPE;
    xma_frame->data[1].is_clone = false;
    xma_frame->data[1].buffer = calloc(1, frame_size_uv);

	memcpy((char*)xma_frame->data[0].buffer,iyBuf, frame_size_y);//y
	memcpy((char*)xma_frame->data[1].buffer, iuvBuf,frame_size_uv);//uv
		
	ret = xma_enc_session_send_frame(enc_ctx->enc_session, enc_ctx->enc_in_frame);
	if (ret == XMA_ERROR)
	{
		printf("enc snd=============== XMA_ERROR \n");
	}
	if (ret == XMA_SUCCESS) {
		enc_ctx->enc_state = ENC_GET_OUTPUT;
	}
	else if(ret == XMA_SEND_MORE_DATA) 
	{
		enc_ctx->enc_state = ENC_READ_INPUT;
	}
	else 
	{
		printf("Encoder send frame failed!!\n");
		return ENC_APP_DONE;
	}
	
	int32_t recv_size = 0;
	ret = xma_enc_session_recv_data(enc_ctx->enc_session, &(enc_ctx->xma_buffer), &recv_size);
	if (ret == XMA_SUCCESS) 
	{
		memcpy(outBuf,enc_ctx->xma_buffer.data.buffer,recv_size);
		*outlen = recv_size;
	} 
	else if(ret <= XMA_ERROR) 
	{
		return ENC_APP_DONE;
	}

    return ENC_APP_SUCCESS;
}

void Encoder_Release(XlnxEncoderCtx *enc_ctx,XmaEncoderProperties *xma_enc_props,XmaFilterProperties *xma_la_props)
{
	xlnx_enc_app_close(enc_ctx, xma_enc_props, xma_la_props);
}

void FPGA_Init()
{
	printf("======FPGA Init====== \n");
	XmaXclbinParameter xclbin_param[4];

	xclbin_param[0].device_id = 0;
	xclbin_param[0].xclbin_name = "/opt/xilinx/xcdr/xclbins/transcode.xclbin";
	xclbin_param[1].device_id = 1;
	xclbin_param[1].xclbin_name = "/opt/xilinx/xcdr/xclbins/transcode.xclbin";

	xclbin_param[2].device_id = 2;
	xclbin_param[2].xclbin_name = "/opt/xilinx/xcdr/xclbins/transcode.xclbin";
	xclbin_param[3].device_id = 3;
	xclbin_param[3].xclbin_name = "/opt/xilinx/xcdr/xclbins/transcode.xclbin";
#if 0
	xclbin_param[4].device_id = 4;
	xclbin_param[4].xclbin_name = "/opt/xilinx/xcdr/xclbins/transcode.xclbin";
	xclbin_param[5].device_id = 5;
	xclbin_param[5].xclbin_name = "/opt/xilinx/xcdr/xclbins/transcode.xclbin";

	xclbin_param[6].device_id = 6;
	xclbin_param[6].xclbin_name = "/opt/xilinx/xcdr/xclbins/transcode.xclbin";
	xclbin_param[7].device_id = 7;
	xclbin_param[7].xclbin_name = "/opt/xilinx/xcdr/xclbins/transcode.xclbin";
#endif
	xma_initialize(xclbin_param, 4);
}