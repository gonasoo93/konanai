/*K
This file is part of Konantech AI Framework project
It is subject to the license terms in the LICENSE file found in the top-level directory of this distribution
*/
#pragma once

#include "../layer.cuh"

class SeqLayer : public Layer {
public:
	SeqLayer(Dict options, Shape& shape, bool& seq, Engine& engine);
	virtual ~SeqLayer();

protected:
	friend class Layer;

	virtual bool seq_layer() { return true; }

	bool m_inseq;
	bool m_outseq;
};

class RnnLayer : public SeqLayer {
public:
	RnnLayer(Dict options, Shape& shape, bool& seq, Engine& engine);
	virtual ~RnnLayer();

protected:
	virtual Array<float> m_forward_farr(Array<float> hidden);
	virtual Array<float> m_backprop_farr(Array<float> G_hidden);

	virtual Array<float> m_forward_cuda_farr(Array<float> hidden);
	virtual Array<float> m_backprop_cuda_farr(Array<float> G_hidden);

	bool m_lstm;
	bool m_use_state;

	int m_recur_size;
	int m_timefeats;
	int m_ex_inp_dim;
	int m_timesteps; // m_xseq == false�� ��쿡�� ������ ��, �ƴϸ� �Է� ũ�⿡�� ȹ��

	List m_rnn_haux; //	����� ���� ���� (�� ���� ���� ������)
	List m_rnn_caux; //	��� ���� ���� (�� ���� ���� ������)

protected:
	virtual int64 dump_structure(int64 depth);
};
