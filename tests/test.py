import unittest
import ROOT
import numpy as np
import torch
from cppyy.gbl.std import vector
import os

project_root = os.path.abspath(os.path.dirname(os.path.dirname(__file__)))

# ROOT.gSystem.Load(os.path.join(project_root, "install/lib/libDDML.so"))
# ROOT.gSystem.Load(os.path.join(project_root, "cmake-build-debug-docker/src/libDDML.so"))
# TODO: PATH for CI, I really need to figure out how to properly set the rpath using ROOT_GENERATE_DICT for 'installed'
#  libDDML. Maybe using the target DDMLROOTDict.cxx somehow?
ROOT.gSystem.Load(os.path.join(project_root, "cmake-build/src/libDDML.so"))


class TheseTests(unittest.TestCase):
    def setUp(self) -> None:
        self.model_path = os.path.join(project_root, 'models/francisca_gan_jit.pt')
        self.batch_size = 1
        self.tensor_opt = {
            'dtype': torch.float32,
            'device': torch.device('cpu')
        }
        self.setup_P()
        self.setup_P_jit()
        self.setup_C()

    def setup_P(self):
        self.noise = torch.as_tensor(np.random.uniform(-1, 1, 100), **self.tensor_opt).view(self.batch_size, 100, 1, 1,
                                                                                            1).detach()
        self.E = torch.full((1,), 90, **self.tensor_opt).view(self.batch_size, 1, 1, 1, 1).detach()

        self.p_input = torch.cat((self.noise.flatten(), self.E.flatten()))
        # print(self.p_input, self.p_input.shape)

    def setup_P_jit(self):
        self.p_jit_model = torch.jit.load(self.model_path)
        # self.p_jit_output = torch.zeros((30*30*30), **self.tensor_opt).detach()

    def setup_C(self):
        self.c_input = vector['float'](self.p_input)
        self.c_output = vector['float'](np.zeros((30 * 30 * 30)))

        self.c_torch_inference = ROOT.ddml.TorchInference()
        self.c_torch_inference.setModelPath(self.model_path)

    def test_jit_outputs(self):
        # TODO: it seems the E_sum of the jitted model differs quite a bit between python ans c++. Figure out why that is the case.
        self.p_jit_output = self.p_jit_model.forward(self.noise, self.E).detach()

        self.c_torch_inference.runInference(self.c_input, self.c_output)
        self.c_output_pythonised = torch.as_tensor(self.c_output, **self.tensor_opt).reshape(30, 30, 30).detach()
        print(np.sum(self.c_output), (self.c_output_pythonised.sum()))
        print(self.p_jit_output.sum(), self.c_output_pythonised.sum())

    def test_p_input(self):
        self.assertTrue((self.noise.flatten() == self.p_input[:-1]).all())
        self.assertTrue((self.E.flatten() == self.p_input[-1]).all())

    def test_numpy_to_c_input(self):
        self.assertTrue((self.p_input.numpy() == self.c_input).all())


if __name__ == "__main__":
    unittest.main()
