/*K
This file is part of Konantech AI Framework project
It is subject to the license terms in the LICENSE file found in the top-level directory of this distribution
*/
#pragma once

typedef int KRetCode;

//#define THROW(x) {  fprintf(stdout, "Kai_cuda error-%d\n", x); assert(0); } 

#define KRetOK	0

#define KWARN_ALREADY_OPENED_LOCAL_LIB					1

// from kai_dummy
#define KERR_UNKNOWN_ERROR							10001
#define KERR_UNIMPEMENTED_YET						10002
#define KERR_WILL_BE_SUPPORTED_IN_NEXT_VERSION		10003
#define KERR_INVALID_SESSION_HANDLE					10004
#define KERR_NULL_HANDLE_USED						10005
#define KERR_INVALIDL_HANDLE_USED					10006
#define KERR_BAD_LOCAL_LIB_NAME						10007
#define KERR_EXISTING_LOCAL_LIB_NAME				10008
#define KERR_UNKNOWN_LOCAL_LIB_NAME					10009
#define KERR_OPENED_LOCAL_LIB						10010
#define KERR_SESSION_CREATE_FAILURE					10011
#define KERR_BROKEN_LOCAL_LIB_FOLDER				10012
#define KERR_BROKEN_LOCAL_LIB_FILE					10013
#define KERR_FAIL_TO_CREATE_LOCAL_LIB_FOLDER		10014
#define KERR_FAIL_TO_OPEN_LOCAL_LIB_NAME			10015
#define KERR_LOCAL_LIB_PASSWORD_MISMATCH			10016
#define KERR_FAIL_TO_SAVE_LOCAL_LIB_FILE			10017
#define KERR_BAD_SESSION_USED						10018
#define KERR_RENAME_TO_OLD_NAME						10019
#define KERR_BAD_NAME_USED							10020
#define KERR_UNKNOWN_INSTALL_MODEL_NAME				10021
#define KERR_FAIL_TO_CREATE_LOCAL_LIB_FILE			10022
#define KERR_FAIL_TO_OPEN_LOCAL_LIB_FILE			10023
#define KERR_BAD_PASSWORD_FORMAT					10024
#define KERR_INTERNAL_ERROR_IN_CLOSING_LOCLIB		10025
#define KERR_NEED_CODE_MODIFICATION					10026
#define KERR_BAD_EMPTY_STRING_FOR_PATH				10027
#define KERR_CANNOT_MOVE_TO_DESCENDANT				10028
#define KERR_CANNOT_DELETE_ROOT_FOLDER				10029
#define KERR_COMPONENT_ALREADY_EXIST				10030
#define KERR_SUBFOLDER_NOT_EXIST					10031
#define KERR_FOLDER_ALREADY_EXIST					10032
#define KERR_NULL_POINTER_USED_FOR_RETURN_VALUE		10033
#define KERR_COMPONENT_NOT_FOUND					10034
#define KERR_COMPONENT_TYPE_MISMATCH				10035
#define KEER_NOT_EXISTING_COMPONENT					10036
#define KERR_UNKNOWN_COMPONENT_TYPE					10037
#define KERR_EMPTY_STRING_IS_NOT_A_PROPER_NAME		10038
#define KERR_BAD_CHAR_USED_FOR_NAME					10039
#define KERR_FAILURE_ON_FILE_SAVE					10040
#define KERR_FAILURE_ON_FILE_READ					10041
#define KERR_DUPLICATED_SUBFOLDER_NAME				10042
#define KERR_ROOT_FOLDER_HAS_NO_PARENT				10043

// from kai_engine
#define KERR_ASSERT									20000
#define KERR_INTERNAL_LOGIC_ERROR					20001
#define KERR_SET_CUDA_OPTION_USAGE_ERROR			20002
#define KERR_SET_CUDA_BAD_DEVICE_NUMBER				20003
#define KERR_SET_IMAGE_OPTION_USAGE_ERROR			20004
#define KERR_SET_SELECT_OPTION_USAGE_ERROR			20005
#define KERR_INPROPER_SELECT_TYPE_NAME				20006
#define KERR_NO_OPTIMZER_SET_DEFAULT_ADAM_IN_USING	20007
#define KERR_UNKNOWN_OPTIMIZER_NAME					20008
#define KERR_UNKNOWN_CALLBACK_TYPE					20009
#define KERR_FAIL_TO_OPEN_LOGFILE					20010
#define KERR_UNKNOWN_DATASET_SUBCLASS_NAME			20011
#define KERR_PRECIATED_FUNCTION						20012
#define KERR_BAD_TYPE_IN_VALUE_CONVERSION			20013
#define KERR_SESSION_MISMTACH_FOR_COMPONENT_HANDLE	20014
#define KERR_ONE_HOT_DATA_EXCEED_SIZE				20015
#define KERR_UNKNOWN_DATALOADER_SUBCLASS_NAME		20016
#define KERR_TVT_DATA_CNT_EXCEEDS_TOTAL_DATA_CNT	20017
#define KERR_UNKNOWN_NETWORK_SUBCLASS_NAME			20018
#define KERR_UNKNOWN_LAYER_SUBCLASS_NAME			20019
#define KERR_INVALID_MAKE_LIST_VALUTE_TYPE			20020
#define KERR_INVALID_MAKE_SHAPE_VALUTE_TYPE			20021
#define KERR_WRONG_NETWORK_OUTPUT_VEC_SIZE			20022
#define KERR_LAYER_INDEX_OUT_OF_RANGE				20023
#define KERR_EXP_WITH_USELESS_TAIL					20024
#define KERR_EXP_UNKNOWN_OPERATOR					20025
#define KERR_EXP_ILLFORMED_OPERANDS					20026
#define KERR_INDEX_OUT_OF_RANGE						20027
#define KERR_INTERNAL_ERR_BAD_OP_AUX_TYPE			20028
#define KERR_ARRAY_TOO_MANY_AXIS_COODS				20029
#define KERR_ARRAY_BAD_AXIS_COOD					20030
#define KERR_FILE_SAVE_INT_FAILURE					20031
#define KERR_FILE_SAVE_FLOAT_FAILURE				20032
#define KERR_FILE_SAVE_STRING_FAILURE				20033
#define KERR_FILE_READ_INT_FAILURE					20034
#define KERR_FILE_READ_FLOAT_FAILURE				20035
#define KERR_FILE_READ_STRING_FAILURE				20036
#define KERR_FILE_OPEN_FAILURE						20037
#define KERR_SAVE_ARRAY_FAILURE						20038
#define KERR_READ_ARRAY_FAILURE						20039
#define KERR_GET_ARRAY_DATA_OUT_OF_BOUND			20040
#define KERR_KEY_NOT_FOUND_ON_GET_POPERTY			20041
#define KERR_MAKE_LIST_FORMAT_LENGTH_MISMATCH		20042
#define KERR_FAILURE_ON_GET_LOCALTIME				20043
#define KERR_UNKNOWN_KEY_IN_GET_PROPERTY			20044
#define KERR_UNSUPPORTED_HANLE_TYPE_IN_SAVE			20045
#define KERR_NO_BOUND_DATASET_FOR_MODEL_EXEC		20046
#define KERR_NO_DATASET_FOR_MODEL_EXEC				20047
#define KERR_UNDEFINED_VAR_USERD_IN_EXP				20048
#define KERR_NO_NETWOK_FOR_MODEL_EXEC				20049
#define KERR_NO_OPTIMIZER_FOR_MODEL_EXEC			20050
#define KERR_MISSING_PROPERTY_ON_MODEL_CONTEXT		20051
#define KERR_BAD_INDEX_ON_ARRAY_SUBRANGE			20052
#define KERR_BAD_INPUT_COLUMN_SPECIFICATION			20053
#define KERR_BAD_OUTPUT_COLUMN_SPECIFICATION		20054
#define KERR_NO_CUDA_DEVICE_IS_AVAILABLE			20055
#define KERR_FAILURE_ON_SETTING_CUDA_DEVICE			20056
#define KERR_CUDA_MALLOC_FAILURE					20057
#define KERR_UNKNOWN_LAYER_NAME_IN_FORWARD			20058
#define KERR_TOO_LARGE_CUT_IN_SHAPE_REPLACE_TAIL	20059
#define KERR_BAD_2D_OPTION_VALUE					20060
#define KERR_SHAPE_MISMATCHT_IN_SHAPE_REPLACE_TAIL	20061
#define KERR_MATMUL_SHAPE_MISMATCH					20062
#define KERR_ADD_BIAS_SHAPE_MISMATCH				20063
#define KERR_FREE_REQUEST_FOR_UNALLOCATED_DEV_MEM	20064
#define KERR_FREE_REQUEST_SIZE_MISMATCH				20065
#define KERR_UNKNOWN_ACTFUNCNAME					20066
#define KERR_NO_LOSS_FUNC_FOR_MODEL_EXEC			20067
#define KERR_INPROPER_NUMBER_OF_OPERANDS			20068
#define KERR_ARRAY_DIVIDE_WITH_NONE_BUNMO			20069
#define KERR_LOSS_FUNCTION_WITH_NONSCALAR_OUTPUT	20070
#define KERR_BAD_INDEX_ON_FARRAY_FETCH				20071
#define KERR_ACC_FUNCTION_WITH_NONSCALAR_OUTPUT		20072
#define KERR_NO_ACCURACY_FUNC_FOR_MODEL_EXEC		20073
#define KERR_BAD_CONSTANT_TYPE_IN_EXPRESSION		20074
#define KERR_UNKNOWN_LAYER_NAME_IN_BACKPROP			20075
#define KERR_BAD_DIMENSION_FOR_ARRAY_TRANSPOSE		20076
#define KERR_BAD_DIMENSION_FOR_ARRAY_SUM_ON_COL		20077
#define KERR_UNSUPPORTED_PARAM_TYPE_FOUND			20078
#define KERR_BAD_SIZE_VECTOR_AS_USER_DATA			20079
#define KERR_BAD_FIELD_NOT_FOUND_IN_USER_DATA		20080
#define KERR_NOT_DIRREFENTIABLE_FUNC_IN_LOSS		20081
#define KERR_REPLACE_REQUEST_ON_EMPTY_SHAPE			20082
#define KERR_CUDA_ERROR								20083
#define KERR_TOO_SHORT_INPUT_SHAPE_FOR_CNN_LAYER	20084
#define KERR_INPUT_SHAPE_MISMATCH					20085
#define KERR_NETWORK_HAS_NO_LAYER					20086
#define KERR_BAD_SHAPE_FOR_CONVOLUTION				20087
#define KERR_FAIL_TO_RESOLVE_MACRO_ARGUMENT			20088
#define KERR_MISMATCHED_SHAPE_IN_PARALLEL_BRANCH	20089
#define KERR_UNKNOWN_SUBNET_TYPE					20090
#define KERR_MISMATCHED_SHAPE_IN_ADD_BRANCH			20091
#define KERR_NO_BRANDCH_IN_PARALLEL_NET				20092
#define KERR_NO_BRANCH_IN_ADD_NET					20093
#define KERR_BRANCH_WITH_BAD_SHAPE_IN_ADD_NET		20094
#define KERR_BRANCH_WITH_BAD_CHANNELS_IN_ADD_NET	20095
#define KERR_UNREGISTED_MACRO_WAS_CALLED			20096
#define KERR_INVALID_JOB_FOR_FEEDING_DATASET		20097
#define KERR_NO_DIRECT_READING_IN_FEEDING_DATASET	20098
#define KERR_NO_ACTIVE_CALLBACK_FUNCTION			20099
#define KERR_DEFAULT_INPUT_SHAPE_NOT_DECLARED		20100
#define KERR_OUTPUT_SHAPE_NOT_DECLARED				20101
#define KERR_DEFAULT_OUTPUT_DTYPE_NOT_FLOAT			20102
#define KERR_INPUT_FEEDING_CALLBACK_UNDEFINED		20103
#define KERR_OUTPUT_FEEDING_CALLBACK_UNDEFINED		20104
#define KERR_RECURRENT_LAYER_WITHOUT_SEQ_DATA		20105
#define KERR_NO_SEQ_DIM_IN_RECURRENT_LAYER_DATA		20106
#define KERR_OUTPUT_TIMESTEPS_UNKNOWN_IN_RNN		20107
#define KERR_CBFUNC_FOR_DATA_FEEFING_UNDEFINED		20108
#define KERR_INVALID_ACCURACY_DICT_CHILD			20109
#define KERR_CHILDREN_OF_BAD_ACC_TERM				20110
#define KERR_ARR_TOKEN_REQUEST_FOR_NOT_ARRAY		20111
#define KERR_INVALID_TOKEN_INFO_FOR_DOWNLOAD_DATA	20112
#define KERR_BAD_BUFFER_SIZE_FOR_DOWNLOAD_DATA		20113
#define KERR_DEFAULT_INPUT_DTYPE_NOT_FLOAT			20114
#define KERR_UNMATCHING_SHAPE_ON_ADD_SERIAL			20115
#define KERR_BAD_EMBED_INFO							20116
#define KERR_BAD_AXIS_FOR_EXTRACT_LAYER				20117
#define KERR_BAD_INDEX_RANGE_FOR_EXTRACT_LAYER		20118
#define KERR_UNMATCHED_OUTPUT_FIELD_SHAPE			20119
#define KERR_FAILURE_ON_DATA_FEEDING_INPUT_FIELD	20120
#define KERR_FAILURE_ON_DATA_FEEDING_OUTPUT_FIELD	20121
#define KERR_SET_ATTR_VALUE_NOT_FOUND				20122
#define KERR_BAD_TYPE_DATA_FOR_SET_ATTR_VALUE		20123
#define KERR_TOO_LARGE_SPLIT_COUNT					20124
#define KERR_INVALID_PIECE_CNT_FOR_SPLIT_ARRAY		20125
#define KERR_FIELD_FOR_SET_ATTRIBUTE_NOT_FOUND		20126
#define KERR_EVALUATE_FUNCTION_NOT_DEFINED			20127
#define KERR_LOSS_FUNCTION_NOT_DEFINED				20128
#define KERR_CHILDREN_OF_BAD_LOSS_TERM				20129
#define KERR_INVALID_LOSS_DICT_CHILD				20130
#define KERR_UNMATCHING_SHAPE_FOR_FILTER_OPERATION	20131
#define KERR_SELCTOR_FOR_SELECT_LAYER_NOT_DEFINED	20132
#define KERR_BAD_SHAPE_PAIR_FOR_SELECT_OP			20133
#define KERR_ANSWER_TENSOR_NOT_FOUND				20134
#define KERR_ESTIMATE_TENSOR_NOT_FOUND				20135
#define KERR_BAD_SHAPE_FOR_REMOVE_TAIL				20136
#define KERR_BAD_SHAPE_FOR_REMOVE_HEAD				20137
#define KERR_SHAPE_TOO_MERGE_SHAPE_NOT_UNIQUE		20138
#define KERR_FAIL_ON_JSON_PARSING					20139
#define KERR_BAD_DIMENSION_FOR_EXPAND_LAYER			20140
#define KERR_FIELD_FOR_STACK_LAYER_NOT_FOUND		20141
#define KERR_BAD_SHAPE_FIELD_FOR_STACK_LAYER		20142
#define KERR_FIELD_DATA_FOR_STACK_LAYER_NOT_FOUND	20143
#define KERR_BAD_RESULT_FOR_STACK_LAYER				20144
#define KERR_SUBEXP_TERM_NOT_FOUND_IN_EXPRESSION	20145
#define KERR_FEEDING_TERM_NOT_FOUND_IN_EXPRESSION	20146
#define KERR_BAD_SHAPE_OPERANDS_FOR_VSTACK			20147
#define KERR_BAD_SHAE_FOR_IOU_OP					20148
#define KERR_DEPRECIATED_FUNCTION_IS_CALLED			20149

#define KERR_NEXT_WORK_HERE							99999
