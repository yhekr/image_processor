#include "../parser/parser.h"
#include "../BMP/BMP.h"
#include "../filters/filters.h"

class Controller {
public:
    Controller(const ParseResults &parse_results) : parse_results_(parse_results) {
    }

    void ApplyFilters();

private:
    ParseResults parse_results_;
};
