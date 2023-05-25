import unittest
import ROOT
import numpy as np
import torch
from cppyy.gbl.std import vector

ROOT.gSystem.Load("../install/lib/libDDML.so")

class TheseTests(unittest.TestCase):
    def setUp(self) -> None:
        self.batch_size = 1
        self.tensor_opt = {
            'dtype': torch.float32,
            'device': torch.device('cpu')
        }
        self.setup_P()
        self.setup_P_jit()
        self.setup_C()

    def setup_P(self):
        self.noise = torch.as_tensor(np.random.uniform(-1, 1, 100), **self.tensor_opt).view(self.batch_size, 100, 1, 1, 1).detach()
        self.E = torch.full((1,), 40, **self.tensor_opt).view(self.batch_size, 1, 1, 1, 1).detach()

        self.p_input = torch.cat((self.noise.flatten(), self.E.flatten()))
    def setup_P_jit(self):
        pass
    def setup_C(self):
        self.c_input = vector['double'](self.p_input)
    def test_numpy_to_c_input(self):
        self.assertTrue((self.p_input.numpy() == self.c_input).all())


if __name__ == "__main__":
    unittest.main()
