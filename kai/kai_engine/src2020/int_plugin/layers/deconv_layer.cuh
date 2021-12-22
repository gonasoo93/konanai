/*K
This file is part of Konantech AI Framework project
It is subject to the license terms in the LICENSE file found in the top-level directory of this distribution
*/
#pragma once

#include "conv_layer.cuh"

class DeconvLayer : public ConvPoolLayer {
public:
	DeconvLayer(Dict options, Shape& shape, bool& seq, Engine& engine);
	virtual ~DeconvLayer();

protected:
	virtual Array<float> m_forward_farr(Array<float> hidden);
	virtual Array<float> m_backprop_farr(Array<float> G_hidden);

	virtual Array<float> m_forward_cuda_farr(Array<float> hidden);
	virtual Array<float> m_backprop_cuda_farr(Array<float> G_hidden);

	virtual int64 dump_structure(int64 depth);
};

