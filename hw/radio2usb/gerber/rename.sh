#/bin/bash

project=sdu

mv *F.Cu.gtl $project.top
mv *B.Cu.gbl $project.bot
mv *F.Mask.gts $project.smt
mv *B.Mask.gbs $project.smb
mv *F.SilkS.gto $project.plt
mv *B.SilkS.gbo $project.plb
mv *Edge.Cuts.gm1 $project.dim
mv *.drl $project.pth
