
#include <python2.7/Python.h>

#include <iostream>
#include <exception>

#include "vector2render.h"

using namespace std;

extern "C" {

	static PyObject * PyVector2Render(PyObject *self, PyObject *args){
	
		int errCode = 0;
	
		int length = 0;
		unsigned char * buffer = NULL;
	
		string input;
		string output;
	
		PyArg_ParseTuple(args, "s#", &buffer, &length);
	
		try{
			input.assign((char *)buffer, length);
			errCode = Vector2Render(input, output);
		}catch(exception & e){
			cout<<"Exception: "<<e.what()<<endl;
		}

		PyObject * pyObject = Py_BuildValue("(s#i)", output.c_str(), output.size());
	
		return pyObject;
	}
	
	static PyMethodDef PyDecoderMethods[] = {
		{"vector2render", 	PyVector2Render, 	METH_VARARGS, 	""},
		{NULL,			NULL,			0,		NULL}
	};
	
	PyMODINIT_FUNC initlibextend(void){
		Py_InitModule("libextend", PyDecoderMethods);
	}

}

