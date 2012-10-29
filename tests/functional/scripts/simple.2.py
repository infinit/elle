#!/usr/bin/env python3
# -*- encoding: utf-8 -*-

import os, sys, time

m1 = os.environ['MNT1']
m2 = os.environ['MNT2']

p1 = os.path.join(m1, 'pif')
p2 = os.path.join(m2, 'pif')

DATA = """bolos /bɔ.los/ masculin, singulier et pluriel identiques

    1. (France) (Argot) Client du marché noir, qui achète de la drogue à un
       dealer[2].

        * […] Pour une paye de braqueur ;
          Ça chuchote, l’État regrimpe, tout se vend ;
          Un bolos passe en coup de vent, la somme dans un coupe-vent […] —
          (Mafia K’1 Fry, Cbr, 2003)

        * […] Les Mc’s parlent mal rêvent de bastos,
          Les khrmal ont pris des balles c’était à Roland Garros,
          Bolos, en émotions le parcours est chargé,
          Largué par l’institution, la nation m’a largué, frelon […] — (Sniper,
          Trait pour trait, 2006)

    2. (France) (Par extension) (Familier) ou (Vulgaire) Injure à la mode, en
       particulier chez les jeunes, synonyme de « gros nul, ringard, bouffon,
       pigeon, victime ».

        * Vas-y t’as vu ta gueule, gros bolos ?

        * Un mec qui sait pas faire un plat c’est carrément un bolosse !
          (réclame télévisuelle pour un contrat de téléphonie mobile)
"""

with open(p1, 'w') as f:
    f.write(DATA.encode("utf8"))

with open(p1, 'r') as f:
    s = f.read().decode("utf8")
    print(s, file=sys.stderr)
    assert s == DATA

MAX_TRIES = 10

tries = 0
while tries < MAX_TRIES:
    if os.path.exists(p2):
        break
    tries += 1
    print("Waiting for the file to be replicated (%d sec)" % tries)
    time.sleep(1)

if tries == MAX_TRIES:
    raise("Maximum number of tries exceeded")

with open(p2, 'r') as f:
    assert f.read() == DATA


