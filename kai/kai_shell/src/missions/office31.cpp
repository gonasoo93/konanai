/*K
This file is part of Konantech AI Framework project
It is subject to the license terms in the LICENSE file found in the top-level directory of this distribution
*/
#include "office31.h"
#include "../../../kai_engine/src/nightly/nightly_utils.h"

// One-hot vector conversion settings : Convert from the column 0 to 3-size one-hot vector.
//#define OFFICE31_MISSION_TO_ONEHOT          KaiList{M,N}	// No use

// Converted data structure : {begin_index, data_count}
#define OFFICE31_MISSION_INPUT_SHAPE        KaiShape{ 100, 100, 3 }
#define OFFICE31_MISSION_OUTPUT_SHAPE       KaiShape{ 3 + 1 + 31 }

#define OFFICE31_MISSION_INPUT_SHAPE_1D     KaiShape{ 100 * 100 * 3}
#define OFFICE31_MISSION_OUTPUT_SHAPE_1D    OFFICE31_MISSION_OUTPUT_SHAPE

// Paths
#define OFFICE31_MISSION_PATH_DATA     ms_data_root + "chap06/office31"
#define OFFICE31_MISSION_PATH_CACHE    ms_cache_root + "office31"

Office31Mission::Office31Mission(KHSession hSession, enum Ken_test_level testLevel) : Mission(hSession, testLevel) {
}

Office31Mission::~Office31Mission() {
}

void Office31Mission::Execute() {
	srand(1234);
	m_createComponents();
	m_execute("Mlp");

	m_dataFeeder.setProperty("input_shape", OFFICE31_MISSION_INPUT_SHAPE);
	m_dataFeeder.setProperty("output_shape", OFFICE31_MISSION_OUTPUT_SHAPE);

	m_execute("Cnn");
	m_execute("Inception", "LA");
	m_execute("Inception", "LAB");
	m_execute("Inception", "LBA");
	m_execute("Resnet", "plain");
	m_execute("Resnet", "residual");
	m_execute("Resnet", "bottleneck");
}

void Office31Mission::m_createComponents() {
	KERR_CHK(KAI_Dataset_create(m_hSession, &m_hDataset, "feeding", { {"name", "office31_dataset"}}));

	m_dataFeeder.ConnectToKai(m_hSession, m_hDataset);

	/*// Urban style
	KERR_CHK(KAI_Dataloader_create(m_hSession, &m_hDataloader, "plain", {}));
	KERR_CHK(KAI_Optimizer_create(m_hSession, &m_hOptimizer, "adam", { {"trace_grad_norm", false}, {"clip_grad", 100000.0f},  {"learning_rate", 0.001f} }));
	KERR_CHK(KAI_Expression_create(m_hSession, &m_hLossExp, "hungarian", { {"exp", "mean(square(sub(@est,@ans)))"} }));
	KERR_CHK(KAI_Expression_create(m_hSession, &m_hAccExp, "hungarian", { {"exp", "sub(1.0, div(sqrt(mean(square(sub(@est,@ans)))), mean(@ans)))"} }));
	KERR_CHK(KAI_Expression_create(m_hSession, &m_hEstExp, "hungarian", { {"exp", "@est"} }));
	*/

	// Office31BuiltinMission style
	KaiDict kwArgs;
	KaiDict lossTerms, accTerms, visTerms;

	kwArgs.clear();
	kwArgs["name"] = "office31_adam";
	kwArgs["ro1"] = 0.9f;
	kwArgs["ro2"] = 0.999f;
	kwArgs["epsilon"] = 1.0e-8f;
	kwArgs["learning_rate"] = 0.001f;
	KERR_CHK(KAI_Optimizer_create(m_hSession, &m_hOptimizer, "adam", kwArgs));

	// Output vector(35) : [3 classes] + [1 class] + [31 classes] => 34 : 3 + 31
	//kwArgs["exp"] = "div( sum( sum(mean(softmax_cross_entropy_with_logits(subvector(0,3,@est),subvector(0,3,@ans))), \
	//				               mean(mult(subvector(3,1,@est),0))), \
	//						   mean(softmax_cross_entropy_with_logits(subvector(4,31,@est),subvector(4,31,@ans)))), \
	//					  2)";
	lossTerms["domain"] = "mean(softmax_cross_entropy_with_logits(subvector(@est, 0, 3), subvector(@ans, 0, 3)))";
	lossTerms["product"] = "mean(softmax_cross_entropy_with_logits(subvector(@est, 4, 31), subvector(@ans, 4, 31)))";

	kwArgs.clear();
	kwArgs["name"] = "office31_loss";	// This "exp" must return data with the same shape as "output_shape".
	kwArgs["terms"] = lossTerms;
	KERR_CHK(KAI_Expression_create(m_hSession, &m_hLossExp, "hungarian", kwArgs));

	accTerms["domain"] = "mean(equal(argmax(subvector(@est, 0, 3)), argmax(subvector(@ans, 0, 3))))";
	accTerms["product"] = "mean(equal(argmax(subvector(@est, 4, 31)), argmax(subvector(@ans, 4, 31))))";

	kwArgs.clear();
	kwArgs["name"] = "office31_acc";
	kwArgs["terms"] = accTerms;
	//kwArgs["exp"] = "div(add( mean(equal(argmax(subvector(0,3,@est)),argmax(subvector(0,3,@ans)))), \
	//					      mean(equal(argmax(subvector(4,31,@est)),argmax(subvector(4,31,@ans)))) ), \
	//	                 2)";
	KERR_CHK(KAI_Expression_create(m_hSession, &m_hAccExp, "hungarian", kwArgs));

	visTerms["domain_probs"] = "mult(softmax(subvector(@est, 0, 3)), 100)";
	visTerms["domain_select"] = "argmax(subvector(@est, 0, 3))";
	visTerms["domain_answer"] = "argmax(subvector(@ans, 0, 3))";

	visTerms["product_probs"] = "mult(softmax(subvector(@est, 4, 31)), 100)";
	visTerms["product_select"] = "argmax(subvector(@est, 4, 31))";
	visTerms["product_answer"] = "argmax(subvector(@ans, 4, 31))";

	kwArgs.clear();
	kwArgs["name"] = "office31_visualize";
	kwArgs["terms"] = visTerms;
	//kwArgs["exp"] = "softmax(@est)";
	KERR_CHK(KAI_Expression_create(m_hSession, &m_hVisExp, "hungarian", kwArgs));

	// Set properties to initialize the data feeder
	kwArgs.clear();
	kwArgs["name"] = "office31_data_feeder";

	// Set the file format
	kwArgs["file_format"] = "image";

	// If the first row is header, then a value of "header_exist" key is true.
	kwArgs["header_exist"] = true;

	// Set the image size to resize
	kwArgs["image_shape"] = OFFICE31_MISSION_INPUT_SHAPE;
	kwArgs["input_shape"] = OFFICE31_MISSION_INPUT_SHAPE_1D;
	kwArgs["output_shape"] = OFFICE31_MISSION_OUTPUT_SHAPE_1D;

	// Shuffle option
#if (ACTIVATE_TEST && TEST_DISABLE_SHUFFLE)
	// no shuffle
	kwArgs["data_split"] = "sequential";
#else
	// shuffle (default)
	kwArgs["data_split"] = "random";
#endif

	// Data split ratio (train | test | validate)
	kwArgs["tr_ratio"] = 0.8f;
	kwArgs["te_ratio"] = 0.15f;
	kwArgs["va_ratio"] = 0.05f;

	// Each batch size (train | test | validate)
	kwArgs["tr_batch_size"] = 10;
	kwArgs["te_batch_size"] = 20;
	kwArgs["va_batch_size"] = 50;

	// Whether to use cache
	kwArgs["load_cache"] = true;
	kwArgs["save_cache"] = false;

	m_dataFeeder.loadData(OFFICE31_MISSION_PATH_DATA, OFFICE31_MISSION_PATH_CACHE, kwArgs,  m_hSession, m_hDataset);
}

KHNetwork Office31Mission::m_buildMlpNetwork() {
	KHNetwork hNetwork = 0;
	KaiDict kwArgs, kwBN, kwParallel, kwDense;

	kwArgs["name"] = "mlp";
	kwArgs["widths"] = KaiList{ 1024, 128, 32 };
	kwArgs["actfunc"] = "relu";
	kwArgs["input_shape"] = OFFICE31_MISSION_INPUT_SHAPE_1D;
	kwArgs["output_shape"] = OFFICE31_MISSION_OUTPUT_SHAPE_1D;
	kwArgs["use_output_layer"] = true;
	kwArgs["init_weight"] = "gaussian";
	//kwArgs["init_weight"] = "adaptive_gaussian";
	kwArgs["init_std"] = 0.03f;

	KERR_CHK(KAI_Network_create(m_hSession, &hNetwork, "mlp", kwArgs));

	return hNetwork;
}

KHNetwork Office31Mission::m_buildCnnNetwork() {
	KHNetwork hNetwork = 0;

	KERR_CHK(KAI_Network_create(m_hSession, &hNetwork, "serial", { {"name", "cnn"}, {"use_output_layer", true} }));

	KERR_CHK(KAI_Network_append_named_layer(m_hSession, hNetwork, "conv", { {"ksize", 3}, {"actfunc", "relu"}, {"init_weight", "gaussian"}, {"init_std", 0.03f}, {"chn", 8} }));
	KERR_CHK(KAI_Network_append_named_layer(m_hSession, hNetwork, "max", { {"stride", 2} }));

	KERR_CHK(KAI_Network_append_named_layer(m_hSession, hNetwork, "conv", { {"ksize", 3}, {"actfunc", "relu"}, {"init_weight", "gaussian"}, {"init_std", 0.03f}, {"chn", 16} }));
	KERR_CHK(KAI_Network_append_named_layer(m_hSession, hNetwork, "max", { {"stride", 2} }));

	KERR_CHK(KAI_Network_append_named_layer(m_hSession, hNetwork, "conv", { {"ksize", 3}, {"actfunc", "relu"}, {"init_weight", "gaussian"}, {"init_std", 0.03f}, {"chn", 32} }));
	KERR_CHK(KAI_Network_append_named_layer(m_hSession, hNetwork, "max", { {"stride", 2} }));

	KERR_CHK(KAI_Network_append_named_layer(m_hSession, hNetwork, "conv", { {"ksize", 3}, {"actfunc", "relu"}, {"init_weight", "gaussian"}, {"init_std", 0.03f}, {"chn", 64} }));
	KERR_CHK(KAI_Network_append_named_layer(m_hSession, hNetwork, "globalavg", {}));

	return hNetwork;
}

void Office31Mission::m_regist_macro_conv1_LA() {
	KHNetwork hMacro = 0;

	KERR_CHK(KAI_Network_create(m_hSession, &hMacro, "serial", { {"name", "conv_LAB"} }));

	KERR_CHK(KAI_Network_append_named_layer(m_hSession, hMacro, "conv", { {"ksize", "#ksize"}, {"chn", "#chn"}, {"stride", "#stride"}, {"actfunc", "relu"}, {"init_weight", "gaussian"}, {"init_std", 0.03f} }));

	KERR_CHK(KAI_Network_regist_macro(m_hSession, hMacro, "conv1"));
}

void Office31Mission::m_regist_macro_conv1_LAB() {
	KHNetwork hMacro = 0;

	KERR_CHK(KAI_Network_create(m_hSession, &hMacro, "serial", { {"name", "conv_LAB"} }));

	KERR_CHK(KAI_Network_append_named_layer(m_hSession, hMacro, "conv", { {"ksize", "#ksize"}, {"chn", "#chn"}, {"stride", "#stride"}, {"actfunc", "relu"}, {"init_weight", "gaussian"}, {"init_std", 0.03f} }));
	KERR_CHK(KAI_Network_append_named_layer(m_hSession, hMacro, "batchnormal", {}));

	KERR_CHK(KAI_Network_regist_macro(m_hSession, hMacro, "conv1"));
}

void Office31Mission::m_regist_macro_conv1_LBA() {
	KHNetwork hMacro = 0;

	KERR_CHK(KAI_Network_create(m_hSession, &hMacro, "serial", { {"name", "conv_LBA"} }));

	KERR_CHK(KAI_Network_append_named_layer(m_hSession, hMacro, "conv", { {"ksize", "#ksize"}, {"chn", "#chn"}, {"stride", "#stride"}, {"actfunc", "none"}, {"init_weight", "gaussian"}, {"init_std", 0.03f} }));
	KERR_CHK(KAI_Network_append_named_layer(m_hSession, hMacro, "batchnormal", {}));
	KERR_CHK(KAI_Network_append_named_layer(m_hSession, hMacro, "pass", { {"actfunc", "relu"} }));

	KERR_CHK(KAI_Network_regist_macro(m_hSession, hMacro, "conv1"));
}

void Office31Mission::m_regist_macro_conv_pair() {
	KHNetwork hMacro = 0;

	KERR_CHK(KAI_Network_create(m_hSession, &hMacro, "serial", { {"name", "conv_pair"} }));

	KERR_CHK(KAI_Network_append_custom_layer(m_hSession, hMacro, "conv1", { {"ksize", "#ksize1"}, {"chn", "#chn1"}, {"stride", "#stride1"} }));
	KERR_CHK(KAI_Network_append_custom_layer(m_hSession, hMacro, "conv1", { {"ksize", "#ksize2"}, {"chn", "#chn2"}, {"stride", "#stride2"} }));

	KERR_CHK(KAI_Network_regist_macro(m_hSession, hMacro, "conv_pair"));
}

void Office31Mission::m_regist_macro_preproc() {
	KHNetwork hMacro = 0;

	KERR_CHK(KAI_Network_create(m_hSession, &hMacro, "serial", { {"name", "flower_preproc"} }));

	KERR_CHK(KAI_Network_append_custom_layer(m_hSession, hMacro, "conv1", { {"ksize", 3}, {"chn", 6}, {"stride", 2} }));

	KERR_CHK(KAI_Network_regist_macro(m_hSession, hMacro, "preproc"));
}

void Office31Mission::m_regist_macro_resize() {
	KHNetwork hMacro = 0;

	KERR_CHK(KAI_Network_create(m_hSession, &hMacro, "parallel", { {"name", "flower_resize"} }));

	KERR_CHK(KAI_Network_append_custom_layer(m_hSession, hMacro, "conv1", { {"ksize", 3}, { "chn", 2 }, { "stride", 2 } }));
	KERR_CHK(KAI_Network_append_custom_layer(m_hSession, hMacro, "conv_pair", { {"ksize1", 3}, {"chn1", 12}, {"stride1", 1}, {"ksize2", 3}, {"chn2", 12}, {"stride2", 2} }));
	KERR_CHK(KAI_Network_append_named_layer(m_hSession, hMacro, "avg", { {"ksize", 3}, {"stride", 2} }));

	KERR_CHK(KAI_Network_regist_macro(m_hSession, hMacro, "resize"));
}

void Office31Mission::m_regist_macro_inception1() {
	KHNetwork hMacro, hBranch = 0;

	KERR_CHK(KAI_Network_create(m_hSession, &hMacro, "parallel", { {"name", "flower_inception1"} }));

	KERR_CHK(KAI_Network_append_custom_layer(m_hSession, hMacro, "conv1", { {"ksize", 1}, { "chn", 4 }, { "stride", 1 } }));

	KERR_CHK(KAI_Network_append_custom_layer(m_hSession, hMacro, "conv1", { {"ksize", 3}, { "chn", 6 }, { "stride", 1 } }));

	KERR_CHK(KAI_Network_append_custom_layer(m_hSession, hMacro, "conv_pair", { {"ksize1", 3}, {"chn1", 6}, {"stride1", 1}, {"ksize2", 3}, {"chn2", 6}, {"stride2", 1} }));

	KERR_CHK(KAI_Network_create(m_hSession, &hBranch, "serial", {}));
	KERR_CHK(KAI_Network_append_named_layer(m_hSession, hBranch, "avg", { {"ksize", 3}, {"stride", 1} }));
	KERR_CHK(KAI_Network_append_custom_layer(m_hSession, hBranch, "conv1", { {"ksize", 1}, { "chn", 4 }, { "stride", 1 } }));
	KERR_CHK(KAI_Network_append_subnet(m_hSession, hMacro, hBranch));

	KERR_CHK(KAI_Network_regist_macro(m_hSession, hMacro, "inception1"));
}

void Office31Mission::m_regist_macro_inception2() {
	KHNetwork hMacro, hBranch, hBranch2;

	KERR_CHK(KAI_Network_create(m_hSession, &hMacro, "parallel", { {"name", "flower_inception2"} }));

	KERR_CHK(KAI_Network_append_custom_layer(m_hSession, hMacro, "conv1", { {"ksize", 1}, { "chn", 8 }, { "stride", 1 } }));

	KERR_CHK(KAI_Network_create(m_hSession, &hBranch, "serial", {}));
	KERR_CHK(KAI_Network_append_custom_layer(m_hSession, hBranch, "conv1", { {"ksize", 3}, { "chn", 8 }, { "stride", 1 } }));
	KERR_CHK(KAI_Network_create(m_hSession, &hBranch2, "parallel", {}));
	KERR_CHK(KAI_Network_append_custom_layer(m_hSession, hBranch2, "conv1", { {"ksize", KaiShape{1,3}}, { "chn", 8 }, { "stride", 1 } }));
	KERR_CHK(KAI_Network_append_custom_layer(m_hSession, hBranch2, "conv1", { {"ksize", KaiShape{3,1}}, { "chn", 8 }, { "stride", 1 } }));
	KERR_CHK(KAI_Network_append_subnet(m_hSession, hBranch, hBranch2));
	KERR_CHK(KAI_Network_append_subnet(m_hSession, hMacro, hBranch));

	KERR_CHK(KAI_Network_create(m_hSession, &hBranch, "serial", {}));
	KERR_CHK(KAI_Network_append_custom_layer(m_hSession, hBranch, "conv1", { {"ksize", 1}, { "chn", 8 }, { "stride", 1 } }));
	KERR_CHK(KAI_Network_append_custom_layer(m_hSession, hBranch, "conv1", { {"ksize", 3}, { "chn", 8 }, { "stride", 1 } }));
	KERR_CHK(KAI_Network_create(m_hSession, &hBranch2, "parallel", {}));
	KERR_CHK(KAI_Network_append_custom_layer(m_hSession, hBranch2, "conv1", { {"ksize", KaiShape{1,3}}, { "chn", 8 }, { "stride", 1 } }));
	KERR_CHK(KAI_Network_append_custom_layer(m_hSession, hBranch2, "conv1", { {"ksize", KaiShape{3,1}}, { "chn", 8 }, { "stride", 1 } }));
	KERR_CHK(KAI_Network_append_subnet(m_hSession, hBranch, hBranch2));
	KERR_CHK(KAI_Network_append_subnet(m_hSession, hMacro, hBranch));

	KERR_CHK(KAI_Network_create(m_hSession, &hBranch, "serial", {}));
	KERR_CHK(KAI_Network_append_named_layer(m_hSession, hBranch, "avg", { {"ksize", 3}, {"stride", 1} }));
	KERR_CHK(KAI_Network_append_custom_layer(m_hSession, hBranch, "conv1", { {"ksize", 1}, { "chn", 8 }, { "stride", 1 } }));

	KERR_CHK(KAI_Network_append_subnet(m_hSession, hMacro, hBranch));

	KERR_CHK(KAI_Network_regist_macro(m_hSession, hMacro, "inception2"));
}

void Office31Mission::m_regist_macro_postproc() {
	KHNetwork hMacro = 0;

	KERR_CHK(KAI_Network_create(m_hSession, &hMacro, "serial", { {"name", "flower_postproc"} }));

	KERR_CHK(KAI_Network_append_named_layer(m_hSession, hMacro, "avg", { {"stride", 6} }));
	KERR_CHK(KAI_Network_append_named_layer(m_hSession, hMacro, "dropout", { {"keep_ratio", 0.7f} }));

	KERR_CHK(KAI_Network_regist_macro(m_hSession, hMacro, "postproc"));
}

KHNetwork Office31Mission::m_buildInceptionNetwork(KString sSubModel) {
	if (sSubModel == "LA") m_regist_macro_conv1_LA();
	else if (sSubModel == "LAB") m_regist_macro_conv1_LAB();
	else if (sSubModel == "LBA") m_regist_macro_conv1_LBA();
	else m_regist_macro_conv1_LAB();

	m_regist_macro_conv_pair();
	m_regist_macro_preproc();
	m_regist_macro_resize();
	m_regist_macro_inception1();
	m_regist_macro_inception2();
	m_regist_macro_postproc();

	KHNetwork hNetwork = 0;

	KERR_CHK(KAI_Network_create(m_hSession, &hNetwork, "serial", { {"name", "inception_flower"}, {"use_output_layer", true} }));

	KERR_CHK(KAI_Network_append_custom_layer(m_hSession, hNetwork, "preproc", {}));
	KERR_CHK(KAI_Network_append_custom_layer(m_hSession, hNetwork, "inception1", {}));
	KERR_CHK(KAI_Network_append_custom_layer(m_hSession, hNetwork, "resize", {}));
	KERR_CHK(KAI_Network_append_custom_layer(m_hSession, hNetwork, "inception1", {}));
	KERR_CHK(KAI_Network_append_custom_layer(m_hSession, hNetwork, "resize", {}));
	KERR_CHK(KAI_Network_append_custom_layer(m_hSession, hNetwork, "inception2", {}));
	KERR_CHK(KAI_Network_append_custom_layer(m_hSession, hNetwork, "resize", {}));
	KERR_CHK(KAI_Network_append_custom_layer(m_hSession, hNetwork, "inception2", {}));
	KERR_CHK(KAI_Network_append_custom_layer(m_hSession, hNetwork, "postproc", {}));

	return hNetwork;
}

void Office31Mission::m_regist_macro_resnet_blocks() {
	m_regist_macro_conv1_LAB();

	KHNetwork hMacro, hBranch;

	KERR_CHK(KAI_Network_create(m_hSession, &hMacro, "serial", {}));
	KERR_CHK(KAI_Network_append_custom_layer(m_hSession, hMacro, "conv1", { {"repeat", "#repeat_p24"}, {"ksize", 3}, {"chn", "#chn"} }));
	KERR_CHK(KAI_Network_append_named_layer(m_hSession, hMacro, "max", { {"stride", 2} }));
	KERR_CHK(KAI_Network_regist_macro(m_hSession, hMacro, "p24"));

	KERR_CHK(KAI_Network_create(m_hSession, &hMacro, "serial", {}));
	KERR_CHK(KAI_Network_append_custom_layer(m_hSession, hMacro, "conv1", { {"ksize", 3}, {"chn", "#chn"}, {"stride", 2} }));
	KERR_CHK(KAI_Network_append_custom_layer(m_hSession, hMacro, "conv1", { {"repeat", "#repeat_pn"}, {"ksize", 3}, {"chn", "#chn"} }));
	KERR_CHK(KAI_Network_regist_macro(m_hSession, hMacro, "pn"));

	KERR_CHK(KAI_Network_create(m_hSession, &hMacro, "add", {}));
	KERR_CHK(KAI_Network_append_custom_layer(m_hSession, hMacro, "conv1", { {"repeat", 2}, {"ksize", 3}, {"chn", "#chn"} }));
	KERR_CHK(KAI_Network_regist_macro(m_hSession, hMacro, "rf"));

	KERR_CHK(KAI_Network_create(m_hSession, &hMacro, "add", { {"stride", 2} }));
	KERR_CHK(KAI_Network_create(m_hSession, &hBranch, "serial", {}));
	KERR_CHK(KAI_Network_append_custom_layer(m_hSession, hBranch, "conv1", { {"ksize", 3}, {"chn", "#chn"}, {"stride", 2} }));
	KERR_CHK(KAI_Network_append_custom_layer(m_hSession, hBranch, "conv1", { {"ksize", 3}, {"chn", "#chn"} }));
	KERR_CHK(KAI_Network_append_subnet(m_hSession, hMacro, hBranch));
	KERR_CHK(KAI_Network_regist_macro(m_hSession, hMacro, "rh"));

	KERR_CHK(KAI_Network_create(m_hSession, &hMacro, "serial", {}));
	KERR_CHK(KAI_Network_append_custom_layer(m_hSession, hMacro, "rf", { {"repeat", "#repeat_rf"} }));
	KERR_CHK(KAI_Network_regist_macro(m_hSession, hMacro, "res_full"));

	KERR_CHK(KAI_Network_create(m_hSession, &hMacro, "serial", {}));
	KERR_CHK(KAI_Network_append_custom_layer(m_hSession, hMacro, "rh", {}));
	KERR_CHK(KAI_Network_append_custom_layer(m_hSession, hMacro, "rf", { {"repeat", "#repeat_rf"} }));
	KERR_CHK(KAI_Network_regist_macro(m_hSession, hMacro, "res_half"));

	KERR_CHK(KAI_Network_create(m_hSession, &hMacro, "add", {}));
	KERR_CHK(KAI_Network_append_custom_layer(m_hSession, hMacro, "conv1", { {"ksize", 1}, {"chn", "#chn1"} }));
	KERR_CHK(KAI_Network_append_custom_layer(m_hSession, hMacro, "conv1", { {"ksize", 3}, {"chn", "#chn1"} }));
	KERR_CHK(KAI_Network_append_custom_layer(m_hSession, hMacro, "conv1", { {"ksize", 1}, {"chn", "#chn4"} }));
	KERR_CHK(KAI_Network_regist_macro(m_hSession, hMacro, "bf"));

	KERR_CHK(KAI_Network_create(m_hSession, &hMacro, "add", { {"stride", 2} }));
	KERR_CHK(KAI_Network_create(m_hSession, &hBranch, "serial", {}));
	KERR_CHK(KAI_Network_append_custom_layer(m_hSession, hBranch, "conv1", { {"ksize", 1}, {"chn", "#chn1"}, {"stride", 2} }));
	KERR_CHK(KAI_Network_append_custom_layer(m_hSession, hBranch, "conv1", { {"ksize", 3}, {"chn", "#chn1"} }));
	KERR_CHK(KAI_Network_append_custom_layer(m_hSession, hBranch, "conv1", { {"ksize", 1}, {"chn", "#chn4"} }));
	KERR_CHK(KAI_Network_append_subnet(m_hSession, hMacro, hBranch));
	KERR_CHK(KAI_Network_regist_macro(m_hSession, hMacro, "bh"));

	KERR_CHK(KAI_Network_create(m_hSession, &hMacro, "serial", {}));
	KERR_CHK(KAI_Network_append_custom_layer(m_hSession, hMacro, "bf", { {"repeat", "#repeat_bf"} }));
	KERR_CHK(KAI_Network_regist_macro(m_hSession, hMacro, "bottle_full"));

	KERR_CHK(KAI_Network_create(m_hSession, &hMacro, "serial", {}));
	KERR_CHK(KAI_Network_append_custom_layer(m_hSession, hMacro, "bh", {}));
	KERR_CHK(KAI_Network_append_custom_layer(m_hSession, hMacro, "bf", { {"repeat", "#repeat_bf"} }));
	KERR_CHK(KAI_Network_regist_macro(m_hSession, hMacro, "bottle_half"));
}

void Office31Mission::m_regist_macro_resnet_model(KString sModel) {
	KHNetwork hMacro = 0;

	KERR_CHK(KAI_Network_create(m_hSession, &hMacro, "serial", { {"name", sModel} }));

	if (sModel == "plain") {
		KERR_CHK(KAI_Network_append_custom_layer(m_hSession, hMacro, "conv1", { {"ksize", 7}, {"stride", 2}, {"chn", 16} }));
		KERR_CHK(KAI_Network_append_named_layer(m_hSession, hMacro, "max", { {"stride", 2} }));
		KERR_CHK(KAI_Network_append_custom_layer(m_hSession, hMacro, "conv1", { {"repeat", 4}, {"ksize", 3}, {"stride", 1}, {"chn", 16} }));
		KERR_CHK(KAI_Network_append_custom_layer(m_hSession, hMacro, "pn", { {"repeat_pn", 3}, {"chn", 32} }));
		KERR_CHK(KAI_Network_append_custom_layer(m_hSession, hMacro, "pn", { {"repeat_pn", 3}, {"chn", 64} }));
		KERR_CHK(KAI_Network_append_named_layer(m_hSession, hMacro, "avg", { {"stride", 4} }));
	}
	else if (sModel == "residual") {
		KERR_CHK(KAI_Network_append_custom_layer(m_hSession, hMacro, "conv1", { {"ksize", 7}, {"stride", 2}, {"chn", 16} }));
		KERR_CHK(KAI_Network_append_named_layer(m_hSession, hMacro, "max", { {"stride", 2} }));
		KERR_CHK(KAI_Network_append_custom_layer(m_hSession, hMacro, "res_full", { {"repeat_rf", 2}, {"chn", 16} }));
		KERR_CHK(KAI_Network_append_custom_layer(m_hSession, hMacro, "res_half", { {"repeat_rf", 1}, {"chn", 32} }));
		KERR_CHK(KAI_Network_append_custom_layer(m_hSession, hMacro, "res_half", { {"repeat_rf", 1}, {"chn", 64} }));
		KERR_CHK(KAI_Network_append_named_layer(m_hSession, hMacro, "avg", { {"stride", 4} }));
	}
	else if (sModel == "bottleneck") {
		KERR_CHK(KAI_Network_append_custom_layer(m_hSession, hMacro, "conv1", { {"ksize", 7}, {"stride", 2}, {"chn", 16} }));
		KERR_CHK(KAI_Network_append_named_layer(m_hSession, hMacro, "max", { {"stride", 2} }));
		KERR_CHK(KAI_Network_append_custom_layer(m_hSession, hMacro, "bottle_full", { {"repeat_bf", 1}, {"chn1", 16}, {"chn4", 64} }));
		KERR_CHK(KAI_Network_append_custom_layer(m_hSession, hMacro, "bottle_half", { {"repeat_bf", 2}, {"chn1", 32}, {"chn4", 128} }));
		KERR_CHK(KAI_Network_append_custom_layer(m_hSession, hMacro, "bottle_half", { {"repeat_bf", 1}, {"chn1", 64}, {"chn4", 256} }));
		KERR_CHK(KAI_Network_append_named_layer(m_hSession, hMacro, "avg", { {"stride", 4} }));
	}

	KERR_CHK(KAI_Network_regist_macro(m_hSession, hMacro, sModel));
}

KHNetwork Office31Mission::m_buildResnetNetwork(KString sSubModel) {
	m_regist_macro_resnet_blocks();
	m_regist_macro_resnet_model(sSubModel);

	KHNetwork hNetwork = 0;

	KERR_CHK(KAI_Network_create(m_hSession, &hNetwork, "serial", { {"use_output_layer", true} }));
	KERR_CHK(KAI_Network_append_custom_layer(m_hSession, hNetwork, sSubModel, {}));

	return hNetwork;
}

KHNetwork Office31Mission::m_buildNetwork(KString sNetwork, KString sSubModel) {
	if (sNetwork == "Mlp") return m_buildMlpNetwork();
	else if (sNetwork == "Cnn") return m_buildCnnNetwork();
	else if (sNetwork == "Inception") return m_buildInceptionNetwork(sSubModel);
	else if (sNetwork == "Resnet") return m_buildResnetNetwork(sSubModel);
	else { assert(0); return NULL; }
}

void Office31Mission::m_execute(KString sModel, KString sSubModel) {
	KString sModelName = "Office31 " + ((sSubModel == "") ? sModel : sModel + "[" + sSubModel + "]");

	printf("\n*** %s Model ***\n\n", sModelName.c_str());

	KHModel hModel = 0;
	KHNetwork hNetwork = m_buildNetwork(sModel, sSubModel);

	KERR_CHK(KAI_Model_create(m_hSession, &hModel, "basic",
		{ {"name", sModelName}, {"dataset", m_hDataset},
		  {"network", hNetwork}, {"loss_exp", m_hLossExp}, {"accuracy_exp", m_hAccExp}, {"visualize_exp", m_hVisExp},
		  {"optimizer", m_hOptimizer}, {"clip_grad", 0.0f} }));

	// Connect to report
	KaiList targetNames = m_dataFeeder.getTargetNames();

	m_reporter.ConnectToKai(m_hSession, hModel, KCb_mask_all);
	m_reporter.setTargetNames(targetNames);

	KERR_CHK(KAI_Model_train(hModel, { {"batch_size", 10}, {"epoch_count", 10}, {"epoch_report", 1}, {"epoch_validate", 5}, {"learning_rate", 0.0001f} }));
	KERR_CHK(KAI_Model_test(hModel, {}));
	KERR_CHK(KAI_Model_visualize(hModel, {}));

	/*
	KaiDict kwPredict;
	KaiList predictResult;

	kwPredict["input_format"] = "image_file_path";
	kwPredict["visualize"] = true;

	kwPredict["userdata"] = KaiList{ "C:/kai2021/data/chap05/flowers/rose/12240303_80d87f77a3_n.jpg" };

	KERR_CHK(KAI_Model_predict(hModel, kwPredict, &predictResult));
	KERR_CHK(KAI_value_dump(predictResult, "predict"));
	*/
}

/*
KHNetwork Office31Mission::m_buildNetwork(KString sCellType) {
	KHNetwork hNetwork = 0;

	// MnistReaderMission style
	//KERR_CHK(KAI_Network_create(m_hSession, &hNetwork, "serial", { {"name", "cnn"}, {"use_output_layer", true} }));
	//KERR_CHK(KAI_Network_append_named_layer(m_hSession, hNetwork, "dense", { {"width", 128} }));
	//KERR_CHK(KAI_Network_append_named_layer(m_hSession, hNetwork, "dense", { {"width", 32} }));
	//KERR_CHK(KAI_Network_append_named_layer(m_hSession, hNetwork, "dense", { {"width", 4} }));
	//KERR_CHK(KAI_Network_append_named_layer(m_hSession, hNetwork, sCellType, { {"width", 27}, {"input_seq", false} , {"output_seq", true} }));

	// Multi-layer perceptron
	KaiDict kwArgs;
	kwArgs["name"] = "office31_mlp";
	kwArgs["widths"] = KaiList{ 1024, 128, 32 };
	kwArgs["actfunc"] = "relu";
	kwArgs["input_shape"] = OFFICE31_MISSION_INPUT_SHAPE_1D;
	kwArgs["output_shape"] = OFFICE31_MISSION_OUTPUT_SHAPE_1D;
	kwArgs["use_output_layer"] = true;
	kwArgs["init_weight"] = "gaussian";
	//kwArgs["init_weight"] = "adaptive_gaussian";
	kwArgs["init_std"] = 0.03f;
	KERR_CHK(KAI_Network_create(m_hSession, &hNetwork, "mlp", kwArgs));

	return hNetwork;
}

void Office31Mission::m_execute(KString sCellType) {
	KString sModelName = "Office31 " + sCellType;

	printf("\n*** %s Model ***\n\n", sModelName.c_str());

	KHModel hModel = 0;
	KHNetwork hNetwork = m_buildNetwork(sCellType);

	KERR_CHK(KAI_Model_create(m_hSession, &hModel, "basic",
		{ {"name", sModelName}, {"dataset", m_hDataset},
		  {"network", hNetwork}, {"loss_exp", m_hLossExp}, {"accuracy_exp", m_hAccExp}, {"visualize_exp", m_hVisExp},
		  {"optimizer", m_hOptimizer}, {"clip_grad", 0.0f} }));

	KERR_CHK(KAI_Model_train(hModel, { {"batch_size", 10}, {"epoch_count", 10}, {"epoch_report", 1}, {"epoch_validate", 5}, {"learning_rate", 0.0001f} }));
	KERR_CHK(KAI_Model_test(hModel, {}));
	KERR_CHK(KAI_Model_visualize(hModel, {}));
}
*/