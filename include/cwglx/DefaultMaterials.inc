#ifndef CW_DEFINE_MATERIAL
#define CW_DEFINE_MATERIAL(NAME, AR, AG, AB, AA, DR, DG, DB, DA, SR, SG, SB, SA, SHINE)
#endif

// These materials come from
// http://www.it.hiof.no/~borres/j3d/explain/light/p-materials.html

// These four materials are of good use
CW_DEFINE_MATERIAL(Brass,
                   0.329412f, 0.223529f, 0.027451f, 1.0f,
                   0.780392f, 0.568627f, 0.113725f, 1.0f,
                   0.992157f, 0.941176f, 0.807843f, 1.0f,
                   27.8974f)
CW_DEFINE_MATERIAL(Chrome,
                   0.25f, 0.25f, 0.25f, 1.0f,
                   0.4f, 0.4f, 0.4f, 1.0f,
                   0.774597f, 0.774597f, 0.774597f, 1.0f,
                   76.8f)
CW_DEFINE_MATERIAL(Tin,
                   0.105882f, 0.058824f, 0.113725f, 1.0f,
                   0.427451f, 0.470588f, 0.541176f, 1.0f,
                   0.333333f, 0.333333f, 0.521569f, 1.0f,
                   9.84615f)
CW_DEFINE_MATERIAL(Plastic,
                   0.2f, 0.2f, 0.2f, 1.0f,
                   0.55f, 0.55f, 0.55f, 1.0f,
                   0.7f, 0.7f, 0.7f, 1.0f,
                   32.0f)

// The following materials looks weird when seen from certain aspect
CW_DEFINE_MATERIAL(Bronze,
                   0.2125f, 0.1275f, 0.054f, 1.0f,
                   0.714f, 0.4284f, 0.18144f, 1.0f,
                   0.393548f, 0.271906f, 0.166721f, 1.0f,
                   25.6f)
CW_DEFINE_MATERIAL(PolishedBronze,
                   0.25f, 0.148f, 0.06475f, 1.0f,
                   0.4f, 0.2368f, 0.1036f, 1.0f,
                   0.774597f, 0.458561f, 0.200621f, 1.0f,
                   76.8f)
CW_DEFINE_MATERIAL(Cobre,
                   0.19125f, 0.0735f, 0.0225f, 1.0f,
                   0.7038f, 0.27048f, 0.0828f, 1.0f,
                   0.256777f, 0.137622f, 0.086014f, 1.0f,
                   12.8f)
CW_DEFINE_MATERIAL(PolishedCobre,
                   0.2295f, 0.08825f, 0.0275f, 1.0f,
                   0.5508f, 0.2118f, 0.066f, 1.0f,
                   0.580594f, 0.223257f, 0.0695701f, 1.0f,
                   51.2f)
CW_DEFINE_MATERIAL(Gold,
                   0.24725f, 0.1995f, 0.0745f, 1.0f,
                   0.75164f, 0.60648f, 0.22648f, 1.0f,
                   0.628281f, 0.555802f, 0.366065f, 1.0f,
                   51.2f)
CW_DEFINE_MATERIAL(PolishedGold,
                   0.24725f, 0.2245f, 0.0645f, 1.0f,
                   0.34615f, 0.3143f, 0.0903f, 1.0f,
                   0.797357f, 0.723991f, 0.208006f, 1.0f,
                   83.2f)

// the glass material comes from
// http://www.it.hiof.no/~borres/j3d/explain/blending/p-blend.html
// and merely modified

CW_DEFINE_MATERIAL(Glass,
                   0.10f, 0.125f, 0.15f, 0.1f,
                   0.10f, 0.1125f, 0.125f, 0.05f,
                   0.0f, 0.0f, 0.0f, 0.0f,
                   128.0f)

// Materials created by Chuigda
CW_DEFINE_MATERIAL(BlackPlastic,
                   0.1f, 0.1f, 0.1f, 1.0f,
                   0.25f, 0.25f, 0.25f, 1.0f,
                   0.7f, 0.7f, 0.7f, 1.0f,
                   32.0f)

CW_DEFINE_MATERIAL(Steel15N20,
                   0.15f, 0.15f, 0.15f, 1.0f,
                   0.25f, 0.25f, 0.25f, 1.0f,
                   0.774597f, 0.774597f, 0.774597f, 1.0f,
                   76.8f)

CW_DEFINE_MATERIAL(PinkPlastic,
                   0.2f, 0.2f, 0.2f, 1.0f,
                   0.63f, 0.47f, 0.5f, 1.0f,
                   0.7f, 0.7f, 0.7f, 1.0f,
                   32.0f)

#undef CW_DEFINE_MATERIAL
