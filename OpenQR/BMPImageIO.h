#pragma once
#include "core.h"
#include <string>
namespace openqr
{
	class BMPImageIO :public IImageIO
	{
	public:
		BMPImageIO();
		template<typename T>
			virtual Matrix<T> ImageRead(const std::string& filePath)override;
		virtual bool ImageSave(const std::string& filePath)override;
		~BMPImageIO();
	//private:
		int data;
	};
}
