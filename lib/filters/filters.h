#include "../BMP/BMP.h"

class Filter {
public:
    virtual BMP Apply(const BMP &bmp) = 0;
};

class FilterWithMatrix : public Filter {
public:
    RedGreenBlue ApplyMatrix(const std::vector<std::vector<int>>& matrix, int x, int y, BMP bmp);
};

class Crop : public Filter {
public:
    Crop(size_t width, size_t height) : width_(width), height_(height) {}

    BMP Apply(const BMP &bmp) override;

private:
    size_t width_;
    size_t height_;
};

class Grayscale : public Filter {
public:
    BMP Apply(const BMP &bmp) override;
};

class Negative : public Filter {
public:
    BMP Apply(const BMP &bmp) override;
};

class Sharpening : public FilterWithMatrix {
public:
    BMP Apply(const BMP &bmp) override;
};

class EdgeDetection : public FilterWithMatrix {
public:
    EdgeDetection(float threshold) : threshold_(threshold) {}

    BMP Apply(const BMP &bmp) override;
private:
    float threshold_;
};

class GaussianBlur : public Filter {
public:
    GaussianBlur(float sigma) : sigma_(sigma) {}

    BMP Apply(const BMP &bmp) override;
private:
    float sigma_;
};

class Brightness : public Filter {
public:
    Brightness(float coef) : coef_(coef) {}

    BMP Apply(const BMP &bmp) override;
private:
    float coef_;
};

class AddFrame : public Filter {
public:
    AddFrame(int thickness) : thickness_(thickness) {}

    BMP Apply(const BMP &bmp) override;
private:
    float thickness_;
};

class Pixelation  : public Filter {
public:
    Pixelation(int numb_of_pixels) : numb_of_pixels_(numb_of_pixels) {}

    BMP Apply(const BMP &bmp) override;
private:
    float numb_of_pixels_;
};

