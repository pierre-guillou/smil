
#include "DMorphMImage.hpp"
#include "DTest.h"

#ifdef WRAP_PYTHON
class Test_Python_Import : public TestCase
{
    virtual void run()
    {
      Py_Initialize();

      PyObject *_main = PyImport_ImportModule("__main__");
      PyObject *globals = PyModule_GetDict(_main);
      
      PyRun_String("import sys", Py_file_input, globals, NULL);
      PyRun_SimpleString((string("sys.path.append(\"") + MORPHEE_LIBRARY_DIR + "\")").c_str());
      PyRun_SimpleString("import MorpheePython as mp");
      PyRun_SimpleString("mIm = mp.createImage(mp.dataCategory.dtScalar, mp.scalarDataType.sdtUINT8)");
      PyRun_SimpleString("mIm.setSize(256,256)");
      PyRun_SimpleString("mIm.setColorInfo(mp.colorInfo.ciMonoSpectral)");
      PyRun_SimpleString("mIm.allocateImage()");

      PyObject *pyobj = PyDict_GetItem(globals, PyString_FromString( "mIm" ));

      MorphmInt<UINT8> mIm(pyobj);
      TEST_ASSERT(mIm.isAllocated());
      
      Py_Finalize();
	
    }
};
#endif // WRAP_PYTHON

int main(int argc, char *argv[])
{
      TestSuite ts;
      
#ifdef WRAP_PYTHON
      ADD_TEST(ts, Test_Python_Import);
#endif // WRAP_PYTHON
      
      return ts.run();
  
}

