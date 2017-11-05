#include "stdafx.h"
#include "ImageProcess.h"


float ImageProcess::a = -0.5;
ImageProcess::ImageProcess(CImage * img, const CString & cstr, int threadNum, bool isCurrent)
{
	this->initImg = img;
	this->mat = new Mat();
	this->cstr = move(cstr);
	this->threadNum = threadNum;
	this->isCurrent = isCurrent;
	this->CImageToMat(*this->initImg, *this->mat);
}

void ImageProcess::MatToCImage(Mat & mat, CImage & cimage)
{
	if (0 == mat.total())
	{
		return;
	}


	int nChannels = mat.channels();
	if ((1 != nChannels) && (3 != nChannels))
	{
		return;
	}
	int nWidth = mat.cols;
	int nHeight = mat.rows;


	//�ؽ�cimage  
	cimage.Destroy();
	cimage.Create(nWidth, nHeight, 8 * nChannels);


	//��������  


	uchar* pucRow;                                  //ָ������������ָ��  
	uchar* pucImage = (uchar*)cimage.GetBits();     //ָ����������ָ��  
	int nStep = cimage.GetPitch();                  //ÿ�е��ֽ���,ע���������ֵ�����и�  


	if (1 == nChannels)                             //���ڵ�ͨ����ͼ����Ҫ��ʼ����ɫ��  
	{
		RGBQUAD* rgbquadColorTable;
		int nMaxColors = 256;
		rgbquadColorTable = new RGBQUAD[nMaxColors];
		cimage.GetColorTable(0, nMaxColors, rgbquadColorTable);
		for (int nColor = 0; nColor < nMaxColors; nColor++)
		{
			rgbquadColorTable[nColor].rgbBlue = (uchar)nColor;
			rgbquadColorTable[nColor].rgbGreen = (uchar)nColor;
			rgbquadColorTable[nColor].rgbRed = (uchar)nColor;
		}
		cimage.SetColorTable(0, nMaxColors, rgbquadColorTable);
		delete[]rgbquadColorTable;
	}


	for (int nRow = 0; nRow < nHeight; nRow++)
	{
		pucRow = (mat.ptr<uchar>(nRow));
		for (int nCol = 0; nCol < nWidth; nCol++)
		{
			if (1 == nChannels)
			{
				*(pucImage + nRow * nStep + nCol) = pucRow[nCol];
			}
			else if (3 == nChannels)
			{
				for (int nCha = 0; nCha < 3; nCha++)
				{
					*(pucImage + nRow * nStep + nCol * 3 + nCha) = pucRow[nCol * 3 + nCha];
				}
			}
		}
	}
}

void ImageProcess::CImageToMat(CImage & cimage, Mat & mat)
{
	if (true == cimage.IsNull())
	{
		return;
	}
	int nChannels = cimage.GetBPP() / 8;
	if ((1 != nChannels) && (3 != nChannels))
	{
		return;
	}
	int nWidth = cimage.GetWidth();
	int nHeight = cimage.GetHeight();
	cout << nWidth << nHeight << endl;

	//�ؽ�mat  
	if (1 == nChannels)
	{
		mat.create(nHeight, nWidth, CV_8UC1);
	}
	else if (3 == nChannels)
	{
		mat.create(nHeight, nWidth, CV_8UC3);
	}


	//��������  


	uchar* pucRow;                               
	uchar* pucImage = (uchar*)cimage.GetBits();     
	int nStep = cimage.GetPitch();                 


	for (int nRow = 0; nRow < nHeight; nRow++)
	{
		pucRow = (mat.ptr<uchar>(nRow));
		for (int nCol = 0; nCol < nWidth; nCol++)
		{
			if (1 == nChannels)
			{
				pucRow[nCol] = *(pucImage + nRow * nStep + nCol);
			}
			else if (3 == nChannels)
			{
				for (int nCha = 0; nCha < 3; nCha++)
				{
					pucRow[nCol * 3 + nCha] = *(pucImage + nRow * nStep + nCol * 3 + nCha);
				}
			}
		}
	}
}

ImageProcess::~ImageProcess()
{
	delete this->mat;
}

CImage* ImageProcess::process_photo()
{
	if (this->cstr == "��������") {
		this->mat = this->AddNoise(this->mat, 1000);
	}
	else if (this->cstr == "��ֵ�˲�") {
		this->mat = this->MedianFilter(this->mat, 4);
	}
	else if (this->cstr == "˫���ײ�ֵ�����ţ�") {
		this->mat = this->zoom(this->mat, 0.5);
	}
	else if (this->cstr == "˫���ײ�ֵ����ת��") {
		this->mat = this->rotate(this->mat, 60);
	}
	else if (this->cstr == "�Զ���ƽ��") {
		this->mat = this->autoBalance(this->mat);
	}
	else if (this->cstr == "�Զ�ɫ��") {
		this->mat = this->autoLevel(this->mat);
	}
	else if (this->cstr == "����Ӧ˫���˲�") {
		this->mat = this->bilateralFilter(this->mat, 4, 10, 10);
	}
	this->img = new CImage();
	this->MatToCImage(*this->mat, *this->img);
	return this->img;
}
//��������
Mat * ImageProcess::AddNoise(Mat * mat, int n)
{
	srand((unsigned)time(NULL));
#pragma omp parallel for num_threads(threadNum)
	for (int i = 0; i < n; ++i) {
		int x1, x2, y1, y2;
		x1 = rand() % mat->rows;
		y1 = rand() % mat->cols;
		x2 = rand() % mat->rows;
		y2 = rand() % mat->cols;
		// һ����ɫͨ���ĻҶ�ͼ
		if (mat->type() == CV_8UC1) {
			mat->at<uchar>(x1, y1) = 0;
			mat->at<uchar>(x2, y2) = 255;
		}
		// ������ɫͨ���ĻҶ�ͼ
		else if (mat->type() == CV_8UC3) {

			mat->at<Vec3b>(x1, y1)[0] = 0;
			mat->at<Vec3b>(x2, y2)[1] = 255;
			mat->at<Vec3b>(x1, y1)[2] = 0;
		}
	}
	return mat;
}
//��ֵ�˲�
Mat * ImageProcess::MedianFilter(Mat * mat, int n)
{

	//��ʽ�� R * 0.144+ 0.587*G +B* 0.299 
	auto distMat = new Mat(mat->rows, mat->cols, mat->type());
	auto within = [&](int x, int y) {
		return x >= 0 && y >= 0 && x < (mat->rows) && y < (mat->cols);
	};
	int allRow = ceil(mat->rows / n);
#pragma omp parallel for num_threads(threadNum)
	for (int row = 0; row < allRow; ++row) {
		for (int col = 0; col < ceil(mat->cols / n); ++col) {
			vector<pair<float, pair<int, int>>> vec;
			int initx = row*n, inity = col*n;
			for (int x = initx; x < initx + n; ++x) {
				for (int y = inity; y < inity + n; ++y) {
					if (within(x, y)) vec.emplace_back(make_pair(0.144*mat->at<Vec3b>(x, y)[0] + 0.587*mat->at<Vec3b>(x, y)[1] + 0.299*mat->at<Vec3b>(x, y)[2], make_pair(x, y)));
				}
			}
			sort(vec.begin(), vec.end());
			auto mid = vec.size() / 2;
			auto actx = vec[mid].second.first, acty = vec[mid].second.second;
			auto pointColor = mat->at<Vec3b>(actx, acty);
			for (int x = initx; x < initx + n; ++x) {
				for (int y = inity; y < inity + n; ++y) {
					if (within(x, y)) distMat->at<Vec3b>(x, y) = pointColor;
				}
			}
		}
	}
	return distMat;
}

// ˫���ײ�ֵ�����ţ�
Mat * ImageProcess::zoom(Mat * mat, float n)
{
	int newRow = mat->rows*n, newCol = mat->cols*n;
	auto within = [&](int x, int y) {
		return x >= 0 && y >= 0 && x < (mat->rows) && y < (mat->cols);
	};
	auto bigMat = new Mat(newRow, newCol, mat->type());
	#pragma omp parallel for num_threads(threadNum)//openmp
	for (int i = 0; i < newRow; i++) {
		for (int j = 0; j < newCol; j++) {
			float x = i / n;
			float y = j / n;
			float w_x[4], w_y[4];//���з���ļ�Ȩϵ��
			getW_x(w_x, x);
			getW_y(w_y, y);
			Vec3f temp = { 0, 0, 0 };
			for (int s = 0; s < 4; s++) {
				for (int t = 0; t < 4; t++) {
					if (within(int(x) + s - 1, int(y) + t - 1))
						temp += (Vec3f)(mat->at<Vec3b>(int(x) + s - 1, int(y) + t - 1))*w_x[s] * w_y[t];
				}
			}
			bigMat->at<Vec3b>(i, j) = move((Vec3b)temp);
		}
	}
	return bigMat;
}
//˫���ײ�ֵ����ת��
// ��ʱ����ת
Mat * ImageProcess::rotate(Mat * mat, float degree)
{
	int newRow = mat->rows*cos(degree*PI / 180) + mat->cols*sin(degree*PI / 180), newCol = mat->rows*sin(degree*PI / 180) + mat->cols*cos(degree*PI / 180);
	auto within = [&](int x, int y) {
		return x >= 0 && y >= 0 && x < (mat->rows) && y < (mat->cols);
	};
	auto bigMat = new Mat(newRow, newCol, mat->type());
	#pragma omp parallel for num_threads(threadNum)//openmp
	for (int i = 0; i < newRow; i++) {
		for (int j = 0; j < newCol; j++) {
			float x = (i - newRow / 2)*cos(degree*PI / 180) + (j - newCol / 2)*sin(degree*PI / 180) + mat->rows / 2;
			float y = -(i - newRow / 2)*sin(degree*PI / 180) + (j - newCol / 2)*cos(degree*PI / 180) + mat->cols / 2;
			float w_x[4], w_y[4];//���з���ļ�Ȩϵ��
			getW_x(w_x, x);
			getW_y(w_y, y);
			Vec3f temp = { 0, 0, 0 };
			for (int s = 0; s < 4; s++) {
				for (int t = 0; t < 4; t++) {
					if (within(int(x) + s - 1, int(y) + t - 1))
						temp += (Vec3f)(mat->at<Vec3b>(int(x) + s - 1, int(y) + t - 1))*w_x[s] * w_y[t];
				}
			}
			bigMat->at<Vec3b>(i, j) = move((Vec3b)temp);
		}
	}
	return bigMat;
}
//�Զ���ƽ��
Mat * ImageProcess::autoBalance(Mat * mat)
{
	double R = 0, G = 0, B = 0;
	#pragma omp parallel for num_threads(threadNum)//openmp
	for (int x = 0; x < mat->rows; ++x) {
		for (int y = 0; y < mat->cols; ++y) {
			auto point = mat->at<Vec3b>(x, y);
			R += point[0];
			G += point[1];
			B += point[2];
		}
	}
	double KR = (R + G + B) / (3 * R), KG = (R + G + B) / (3 * G), KB = (R + G + B) / (3 * B);
	for (int x = 0; x < mat->rows; ++x) {
		for (int y = 0; y < mat->cols; ++y) {
			auto& point = mat->at<Vec3b>(x, y);
			point[0] = min(int(point[0] * KR),255);
			point[1] = min(int(point[1] * KG),255);
			point[2] = min(int(point[2] * KB),255);
		}
	}
	return this->mat;
}
//�Զ�ɫ��
Mat * ImageProcess::autoLevel(Mat * mat)
{
	int maxx[3] = { 0,0,0 }, minn[3] = { 255,255,255 };
	#pragma omp parallel for num_threads(threadNum)//openmp
	for (int x = 0; x < mat->rows; ++x) {
		for (int y = 0; y < mat->cols; ++y) {
			auto& point = mat->at<Vec3b>(x, y);
			for (auto i = 0; i < 3; ++i) {
				if (point[i] > maxx[i]) {
					maxx[i] = point[i];
				}
				if (point[i] < minn[i]) {
					minn[i] = point[i];
				}
			}
		}
	}
	for (int x = 0; x < mat->rows; ++x) {
		for (int y = 0; y < mat->cols; ++y) {
			auto& point = mat->at<Vec3b>(x, y);
			for (auto i = 0; i < 3; ++i) {
				if (maxx[i] != minn[i]) {
					point[i] = float(point[i] - minn[i]) / (maxx[i] - minn[i]) * 255;
				}
			}
		}
	}
	return mat;
}
//����Ӧ˫���˲�

Mat * ImageProcess::bilateralFilter(Mat * mat, int d, double sigmaColor, double sigmaSpace)
{
	auto within = [&](int x, int y) {
		return x >= 0 && y >= 0 && x < (mat->rows) && y < (mat->cols);
	};
	auto spaceFunction = [&](int x, int y, int xx, int yy) {
		return -((x - xx)*(x - xx) + (y - yy)*(y - yy)) / (2 * sigmaSpace*sigmaSpace);
	};
	auto colorFunction = [&](int a, int b) {
		return -(a - b)*(a - b) / (2 * sigmaColor*sigmaColor);
	};
	auto distMat = new Mat(mat->rows, mat->cols, mat->type());
	if (d % 2 == 0) d += 1;
	#pragma omp parallel for num_threads(threadNum)
	for (int x = 0; x < mat->rows; ++x) {
		for (int y = 0; y < mat->cols; ++y) {
			auto& centerPoint = mat->at<Vec3b>(x, y);
			double sums[3] = { 0,0,0 };
			double color[3] = { 0,0,0 };
			for (int xx = x - d / 2; xx <= x + d / 2; ++xx) {
				for (int yy = y - d / 2; yy <= y + d / 2; ++yy) {
					if (within(xx, yy)) {
						auto& point = mat->at<Vec3b>(xx, yy);
						for (int i = 0; i < 3; ++i) {
							double d = spaceFunction(x, y, xx, yy);
							double r = colorFunction(point[i], centerPoint[i]);
							double w = exp(d + r);
							color[i] += point[i] * w;
							sums[i] += w;
						}
					}
				}
			}
			for (int i = 0; i < 3; ++i) {
				color[i] /= sums[i];
				color[i] = max(0, int(color[i]));
				color[i] = min(255, int(color[i]));
			}
			distMat->at<Vec3b>(x, y) = Vec3b{ uchar(color[0]),uchar(color[1]),uchar(color[2]) };
		}
	}
	return distMat;
}

void ImageProcess::getW_x(float w_x[4], float x)
{
	int X = (int)x;//ȡ��������
	float stemp_x[4];
	stemp_x[0] = 1 + (x - X);
	stemp_x[1] = x - X;
	stemp_x[2] = 1 - (x - X);
	stemp_x[3] = 2 - (x - X);

	w_x[0] = a*abs(stemp_x[0] * stemp_x[0] * stemp_x[0]) - 5 * a*stemp_x[0] * stemp_x[0] + 8 * a*abs(stemp_x[0]) - 4 * a;
	w_x[1] = (a + 2)*abs(stemp_x[1] * stemp_x[1] * stemp_x[1]) - (a + 3)*stemp_x[1] * stemp_x[1] + 1;
	w_x[2] = (a + 2)*abs(stemp_x[2] * stemp_x[2] * stemp_x[2]) - (a + 3)*stemp_x[2] * stemp_x[2] + 1;
	w_x[3] = a*abs(stemp_x[3] * stemp_x[3] * stemp_x[3]) - 5 * a*stemp_x[3] * stemp_x[3] + 8 * a*abs(stemp_x[3]) - 4 * a;
}

void ImageProcess::getW_y(float w_y[4], float y)
{
	int Y = (int)y;
	float stemp_y[4];
	stemp_y[0] = 1.0 + (y - Y);
	stemp_y[1] = y - Y;
	stemp_y[2] = 1 - (y - Y);
	stemp_y[3] = 2 - (y - Y);

	w_y[0] = a*abs(stemp_y[0] * stemp_y[0] * stemp_y[0]) - 5 * a*stemp_y[0] * stemp_y[0] + 8 * a*abs(stemp_y[0]) - 4 * a;
	w_y[1] = (a + 2)*abs(stemp_y[1] * stemp_y[1] * stemp_y[1]) - (a + 3)*stemp_y[1] * stemp_y[1] + 1;
	w_y[2] = (a + 2)*abs(stemp_y[2] * stemp_y[2] * stemp_y[2]) - (a + 3)*stemp_y[2] * stemp_y[2] + 1;
	w_y[3] = a*abs(stemp_y[3] * stemp_y[3] * stemp_y[3]) - 5 * a*stemp_y[3] * stemp_y[3] + 8 * a*abs(stemp_y[3]) - 4 * a;
}
CImage * ImageProcess::merge(CImage * src, CImage * dist, double alpha)
{

	auto srcMat = new Mat(), distMat = new Mat();
	CImageToMat(*src, *srcMat);
	CImageToMat(*dist, *distMat);
	auto point_within = [&](int x, int y) {
		return x >= 0 && y >= 0 && x < (srcMat->rows) && y < (srcMat->cols);
	};
	for (int x = 0; x < distMat->rows; ++x) {
		for (int y = 0; y < distMat->cols; ++y) {
			if (point_within(x, y)) {
				auto &srcPoint = srcMat->at<Vec3b>(x, y);
				auto &distPoint = distMat->at<Vec3b>(x, y);
				for (int i = 0; i < 3; ++i) {
					distPoint[i] = min(int(alpha*srcPoint[i] + (1 - alpha)*distPoint[i]), 255);
				}
			}
		}
	}
	auto img = new CImage();
	MatToCImage(*distMat, *img);
	return img;
}