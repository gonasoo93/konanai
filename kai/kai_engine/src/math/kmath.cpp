/*K
This file is part of Konantech AI Framework project
It is subject to the license terms in the LICENSE file found in the top-level directory of this distribution
*/
#include "kmath.h"
#include "khostmath.h"
#include "kcudamath.h"
#include "../gpu_cuda/cuda_manager.h"

class KaiDeviceManager;

KaiMath* KaiMath::GetHostMath() {
	return &hostmath;
}

KaiMath* KaiMath::Allocate(KaiModelInstance* pModelContext) {
	KaiDeviceManager* pDevManager = cuda_man.alloc_device(pModelContext);
	// ���� device �˻� ���� free memory ���� ���� device ������ �ش� devide���� ���� �����ϴ� cudamath �켱 �����ϴ� ������ ����
	return new KaiCudaMath(pDevManager);
}
