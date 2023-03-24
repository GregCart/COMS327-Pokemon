#ifndef ENUMS_H
#define ENUMS_H


typedef enum terrain_e {
    RCK,
    TRE,
    GT,
    WTR,
    GS,
    GTE,
    PTH,
    MRT,
    CTR,
    BDR,
    num_types_ter
} Terrain_e;

typedef enum trainer_e {
    PLAY,
    HIKR,
    RIVL,
    PACR,
    WAND,
    SENT,
    EXPL,
    SWIM,
    num_types_tra
} Trainer_e;

typedef enum direction_e{
    NONE,
    NE,
    N,
    NW,
    W,
    E,
    SW,
    S,
    SE,
    num_dir
} Dir_e;

#endif