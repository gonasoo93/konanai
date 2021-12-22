/*K
This file is part of Konantech AI Framework project
It is subject to the license terms in the LICENSE file found in the top-level directory of this distribution
*/
#include "new_optimizer.cuh"


//hs.cho
#ifndef KAI2021_WINDOWS
#define sprintf_s snprintf
#endif

// ������: �ʿ��� �������� �ִ� ��� ���޹��� kwArgs ������ ������ ����
TemplateOptimizer::TemplateOptimizer(Dict kwArgs) : Optimizer("new", kwArgs) {
	m_test = (float)Value::seek_option(kwArgs, "test", 0.9f);	// �˸��� ����Ʈ ��(ex.0.9f)�� �˷��ָ鼭 �ʿ��� ������ �Ķ���͸� ì��ϴ�.
}

TemplateOptimizer::~TemplateOptimizer() {
}

void TemplateOptimizer::setup(Dict kwArgs) {
	m_test = (float)Value::seek_option(kwArgs, "test", m_test);	// �����ڿ��� ������ ���� ���� ����Ʈ ������ �˷��ָ鼭 �ʿ��� ������ �Ķ���͸� ì��ϴ�.
}

// shell�� optimizer �Ұ� ������ �߰��� �������Ķ���� ���� ����մϴ�.
string TemplateOptimizer::introduce_extra() {
	char buffer[128];
	sprintf_s(buffer,128,", test:%f", m_test);
	return  (string)buffer;
}

void TemplateOptimizer::m_alloc_affine_param(Dict& param, Shape shape, bool use_cuda, Dict kwArgs) {
	// Optimizer::alloc_affine_para() �Լ�m���� �Ķ���� �⺻ ������ ������ �� ȣ���մϴ�.
	// ����� �� �߰� �Ķ���Ͱ� �ʿ��� ��� �� �Լ��� �������� param ������ �����ϰų� �߰��ϼ���.
	// �߰����� ó���� �ʿ����� ���� ��� �Լ� ���� �� ���Ǹ� �����ϰų� �� ���Ǹ� �� �Լ��� �μ���.
}

void TemplateOptimizer::m_alloc_embed_param(Dict& param, vector<int64> voc_sizes, int64 vec_size, Dict kwArgs) {
	// Optimizer::alloc_embed_param() �Լ����� �Ķ���� �⺻ ������ ������ �� ȣ���մϴ�.
	// ����� �� �߰� �Ķ���Ͱ� �ʿ��� ��� �� �Լ��� �������� param ������ �����ϰų� �߰��ϼ���.
	// �߰����� ó���� �ʿ����� ���� ��� �Լ� ���� �� ���Ǹ� �����ϰų� �� ���Ǹ� �� �Լ��� �μ���.
}

void TemplateOptimizer::m_forward_affine(Dict param, Array<float> x, Array<float>& output) {
	// Optimizer::forward_affine() �Լ����� �Ķ���͸� �̿��� ���� ���� ������ ó���� ������ �� ȣ���մϴ�.
	// ó�� ����� ������ �ʿ��ϰų� ���� ���� �� �ٸ� �߰����� ó���� �ʿ��� ���� �� �Լ����� �˸��� �۾��� �����ϼ���.
	// �߰����� ó���� �ʿ����� ���� ��� �Լ� ���� �� ���Ǹ� �����ϰų� �� ���Ǹ� �� �Լ��� �μ���.
}

void TemplateOptimizer::m_backprop_affine(Dict param, Array<float> x, Array<float> G_affine, Array<float>& G_input) {
	// Optimizer::backprop_affine() �Լ����� �Ķ���͸� �̿��� ���� ���� ������ ó���� ������ �� ȣ���մϴ�.
	// ó�� ����� ������ �ʿ��ϰų� ���� ���� �� �ٸ� �߰����� ó���� �ʿ��� ���� �� �Լ����� �˸��� �۾��� �����ϼ���.
	// �߰����� ó���� �ʿ����� ���� ��� �Լ� ���� �� ���Ǹ� �����ϰų� �� ���Ǹ� �� �Լ��� �μ���.
}

void TemplateOptimizer::m_forward_embed(Dict param, Array<int64> selector, Array<float>& output) {
	// Optimizer::forward_embed() �Լ����� �Ķ���͸� �̿��� �Ӻ��� ���� ������ ó���� ������ �� ȣ���մϴ�.
	// ó�� ����� ������ �ʿ��ϰų� ���� ���� �� �ٸ� �߰����� ó���� �ʿ��� ���� �� �Լ����� �˸��� �۾��� �����ϼ���.
	// �߰����� ó���� �ʿ����� ���� ��� �Լ� ���� �� ���Ǹ� �����ϰų� �� ���Ǹ� �� �Լ��� �μ���.
}

void TemplateOptimizer::m_forward_embed_cuda(Dict param, Array<float> word_vecs, Array<int64> selector) {
	// Optimizer::forward_embed_cuda() �Լ����� �Ķ���͸� �̿��� �Ӻ��� ���� ������ ó���� ������ �� ȣ���մϴ�.
	// ó�� ����� ������ �ʿ��ϰų� ���� ���� �� �ٸ� �߰����� ó���� �ʿ��� ���� �� �Լ����� �˸��� �۾��� �����ϼ���.
	// �߰����� ó���� �ʿ����� ���� ��� �Լ� ���� �� ���Ǹ� �����ϰų� �� ���Ǹ� �� �Լ��� �μ���.
}

void TemplateOptimizer::update_weight(Dict pm_w, Array<float> G_weight) {
	// �˰��� �°� ���� ������ ����ġ �Ķ���Ϳ� ���� ������Ʈ�� ����� ������� �����մϴ�.
}

void TemplateOptimizer::update_bias(Dict pm_b, Array<float> G_bias) {
	// �˰��� �°� ���� ������ ���̾ �Ķ���Ϳ� ���� ������Ʈ�� ����� ������� �����մϴ�.
}

void TemplateOptimizer::update_embed(Dict param, Array<float> G_Words, Array<int64> selector) {
	// �˰��� �°� �Ӻ��� ������ ���� �Ķ���Ϳ� ���� ������Ʈ�� ����� ������� �����մϴ�.
}

// �ʿ��� Ŀ�� �Լ� ����
// �Ʒ��� �� �Լ����� �̿��ϴ� Ŀ�� �Լ����� �����մϴ�.

void TemplateOptimizer::update_weight_cuda(Dict param, Array<float> G_weight) {
	// �˰��� �°� ���� ������ ����ġ �Ķ���Ϳ� ���� ������Ʈ�� ��� ������� �����մϴ�.
}

void TemplateOptimizer::update_bias_cuda(Dict param, Array<float> G_bias) {
	// �˰��� �°� ���� ������ ���̾ �Ķ���Ϳ� ���� ������Ʈ�� ��� ������� �����մϴ�.
}

void TemplateOptimizer::update_embed_cuda(Dict param, Array<float> G_words, Array<int64> selector) {
	// �˰��� �°� �Ӻ��� ������ ���� �Ķ���Ϳ� ���� ������Ʈ�� ��� ������� �����մϴ�.
}

// Ŀ�� �Լ� �� ����̽� �Լ� ����
// �տ��� ������ Ŀ�� �Լ��� �����մϴ�. �̵� �Լ����� ȣ���ϴ� ����̽� �Լ��� �ִ� ��� �̵鵵 �����մϴ�.
