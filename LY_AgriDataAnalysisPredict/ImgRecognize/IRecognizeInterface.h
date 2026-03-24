#ifndef IRECOGNIZEINTERFACE_H_
#define IRECOGNIZEINTERFACE_H_
 
namespace recognize
{
	class IRecognizeInterface
	{
	public:
		IRecognizeInterface() = default;
		virtual ~IRecognizeInterface() {}

		virtual bool Recognize(const QImage &originImage, QImage &recoginzedImage, QString &recoginzedName, float threshold = 0.2) = 0;

	private:
		IRecognizeInterface(const IRecognizeInterface &) = delete;
		IRecognizeInterface(const IRecognizeInterface &&) = delete;
		IRecognizeInterface &operator=(const IRecognizeInterface &) = delete;
		IRecognizeInterface &operator=(const IRecognizeInterface &&) = delete;
	};
}
#endif  // IRecognizeInterface_H