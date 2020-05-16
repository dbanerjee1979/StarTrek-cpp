#include "Empty.h"

bool Empty::is_empty() const {
    return true;
}

void Empty::accept(SectorVisitor& visitor) {
    visitor.visit_empty(*this);
}

void Empty::torpedo_hit() {
}
