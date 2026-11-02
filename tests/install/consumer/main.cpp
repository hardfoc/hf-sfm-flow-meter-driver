// Built against the installed headers only: sfm.hpp and the generated sfm_version.h.
#include "sfm.hpp"

#include <cstdio>

int main() {
    std::printf("%s %s\n", sfm::GetDriverVersion(), sfm::ToString(sfm::Variant::Sfm3019_240).data());
    return sfm::Crc8Word(0xBEEF) == 0x92 ? 0 : 1;
}
