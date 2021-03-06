float softplusActivation(float x) { return log(1 + exp(x)); }

void lenModel(float n_0[3], out float n_3[2]) {
	float n_1[8];
	float n_2[8];
	n_1[0] = softplusActivation(n_0[0] * 0.74334157 + n_0[1] * 1.0333363 + n_0[2] * 0.7755172 + -1.3867474);
	n_1[1] = softplusActivation(n_0[0] * 4.9185452 + n_0[1] * -1.8204012 + n_0[2] * 5.3600616 + -5.877275);
	n_1[2] = softplusActivation(n_0[0] * 0.1372628 + n_0[1] * 1.6060426 + n_0[2] * 0.2157267 + -0.9639606);
	n_1[3] = softplusActivation(n_0[0] * -0.034455657 + n_0[1] * 0.22513379 + n_0[2] * -0.01590908 + 2.1601841);
	n_1[4] = softplusActivation(n_0[0] * 2.1818147 + n_0[1] * 0.4775212 + n_0[2] * 2.2646003 + -5.764138);
	n_1[5] = softplusActivation(n_0[0] * 0.8573638 + n_0[1] * 0.29726264 + n_0[2] * 0.5760691 + -3.3737676);
	n_1[6] = softplusActivation(n_0[0] * -0.0015747729 + n_0[1] * 0.1535615 + n_0[2] * 0.005690021 + 0.7526526);
	n_1[7] = softplusActivation(n_0[0] * 0.48818797 + n_0[1] * 2.4172616 + n_0[2] * 0.48910826 + -5.2019672);
	n_2[0] = softplusActivation(n_1[0] * -0.28494588 + n_1[1] * -1.1652126 + n_1[2] * -0.030496377 + n_1[3] * 0.56068593 + n_1[4] * 0.35248986 + n_1[5] * -0.40401733 + n_1[6] * 0.03147413 + n_1[7] * -0.41293734 + -0.31486446);
	n_2[1] = softplusActivation(n_1[0] * 2.5119193 + n_1[1] * -4.6131597 + n_1[2] * -1.3368329 + n_1[3] * 0.5986233 + n_1[4] * 3.0410824 + n_1[5] * -1.3135092 + n_1[6] * -2.8634012 + n_1[7] * -1.4346937 + -1.9644625);
	n_2[2] = softplusActivation(n_1[0] * -0.9420652 + n_1[1] * 0.4062185 + n_1[2] * -1.4458181 + n_1[3] * -0.5570134 + n_1[4] * -0.3809267 + n_1[5] * 0.43844676 + n_1[6] * -1.1804739 + n_1[7] * -1.3371419 + -0.32870755);
	n_2[3] = softplusActivation(n_1[0] * -0.8179669 + n_1[1] * -3.3975787 + n_1[2] * 0.1329131 + n_1[3] * 3.0153205 + n_1[4] * 2.498166 + n_1[5] * 0.097443804 + n_1[6] * -3.7767148 + n_1[7] * -0.6816936 + 0.123290524);
	n_2[4] = softplusActivation(n_1[0] * 0.14837961 + n_1[1] * 0.7749539 + n_1[2] * 0.015072812 + n_1[3] * -3.2134237 + n_1[4] * -1.2151089 + n_1[5] * -0.7249322 + n_1[6] * 5.7942014 + n_1[7] * -1.1327937 + 0.018006843);
	n_2[5] = softplusActivation(n_1[0] * -0.834332 + n_1[1] * 0.3695792 + n_1[2] * -1.112494 + n_1[3] * -0.94543433 + n_1[4] * -0.18943125 + n_1[5] * -0.0092258435 + n_1[6] * -0.23890083 + n_1[7] * -1.2122718 + -0.3161428);
	n_2[6] = softplusActivation(n_1[0] * 1.131507 + n_1[1] * -0.022344412 + n_1[2] * -1.1187752 + n_1[3] * 1.2875402 + n_1[4] * 0.05804678 + n_1[5] * -2.6131945 + n_1[6] * -2.1086104 + n_1[7] * 1.8684769 + -2.7027507);
	n_2[7] = softplusActivation(n_1[0] * 0.41571027 + n_1[1] * -0.35728505 + n_1[2] * -1.1060696 + n_1[3] * -0.9465462 + n_1[4] * 1.1384629 + n_1[5] * -1.1851397 + n_1[6] * -0.44986722 + n_1[7] * 0.60969186 + -0.13775955);
	n_3[0] = n_2[0] * -0.47721684 + n_2[1] * -0.4558794 + n_2[2] * -0.95721304 + n_2[3] * 0.17180803 + n_2[4] * 0.31506565 + n_2[5] * 0.97610885 + n_2[6] * 0.45736083 + n_2[7] * -0.33541548 + -0.010191337;
	n_3[1] = n_2[0] * 8.864783 + n_2[1] * -21.018024 + n_2[2] * 0.07353667 + n_2[3] * -12.5361395 + n_2[4] * 0.0690925 + n_2[5] * -0.13878812 + n_2[6] * -24.681995 + n_2[7] * -0.008305752 + -1.9365352;
}

void pathModel(float n_0[6], out float n_3[6]) {
	float n_1[12];
	float n_2[12];
	n_1[0] = softplusActivation(n_0[0] * -0.007354563 + n_0[1] * -1.579241 + n_0[2] * -17.707178 + n_0[3] * -0.23908241 + n_0[4] * -0.21289288 + n_0[5] * -5.2927884e-05 + 1.4797283);
	n_1[1] = softplusActivation(n_0[0] * -0.011625091 + n_0[1] * -1.7288339 + n_0[2] * 0.9109927 + n_0[3] * -0.05693393 + n_0[4] * -1.537906 + n_0[5] * -2.2551849e-05 + 2.2617273);
	n_1[2] = softplusActivation(n_0[0] * 0.053239834 + n_0[1] * 3.2886026 + n_0[2] * -17.702126 + n_0[3] * 1.1782924 + n_0[4] * 1.1774405 + n_0[5] * 0.00029463824 + -0.5573405);
	n_1[3] = softplusActivation(n_0[0] * -0.014053344 + n_0[1] * 2.1967006 + n_0[2] * 1.1587838 + n_0[3] * -0.03155369 + n_0[4] * 1.3965791 + n_0[5] * 0.0003018705 + -1.3740039);
	n_1[4] = softplusActivation(n_0[0] * -0.009805284 + n_0[1] * -2.238879 + n_0[2] * 0.7480195 + n_0[3] * -0.059656482 + n_0[4] * -1.4483091 + n_0[5] * 7.729805e-05 + 0.0054502357);
	n_1[5] = softplusActivation(n_0[0] * -0.012180893 + n_0[1] * -3.2916393 + n_0[2] * 0.90148515 + n_0[3] * -0.082076825 + n_0[4] * -0.6390684 + n_0[5] * -7.3599396e-05 + 1.3515337);
	n_1[6] = softplusActivation(n_0[0] * 0.011958447 + n_0[1] * 0.3353625 + n_0[2] * -0.6998016 + n_0[3] * 0.008000484 + n_0[4] * 0.15726966 + n_0[5] * 0.00010093431 + 0.8953023);
	n_1[7] = softplusActivation(n_0[0] * -0.0033647674 + n_0[1] * -3.909873 + n_0[2] * -1.7291346 + n_0[3] * -1.3854493 + n_0[4] * -0.9364129 + n_0[5] * 8.396155e-05 + 2.0657184);
	n_1[8] = softplusActivation(n_0[0] * 0.0087475395 + n_0[1] * 3.7238643 + n_0[2] * -0.7320722 + n_0[3] * 0.0088564195 + n_0[4] * 0.90914965 + n_0[5] * -0.00038705522 + -2.9470692);
	n_1[9] = softplusActivation(n_0[0] * -0.03567783 + n_0[1] * 0.58802396 + n_0[2] * -8.365682 + n_0[3] * -0.101885624 + n_0[4] * -0.9674019 + n_0[5] * 9.1367925e-05 + 0.24455431);
	n_1[10] = softplusActivation(n_0[0] * -0.027797826 + n_0[1] * -0.7298109 + n_0[2] * 2.247905 + n_0[3] * -0.10191255 + n_0[4] * 1.0516585 + n_0[5] * -0.00028706272 + 0.87314934);
	n_1[11] = softplusActivation(n_0[0] * -0.00879485 + n_0[1] * -2.208393 + n_0[2] * -0.50012344 + n_0[3] * -1.4367121 + n_0[4] * -0.035694964 + n_0[5] * 1.2371591e-05 + -1.0892198);
	n_2[0] = softplusActivation(n_1[0] * 1.5531114 + n_1[1] * 0.0039376947 + n_1[2] * -2.2978404 + n_1[3] * -0.1357715 + n_1[4] * -0.27852678 + n_1[5] * -0.5186132 + n_1[6] * -0.3675174 + n_1[7] * 0.27162012 + n_1[8] * -0.7511547 + n_1[9] * 0.17377986 + n_1[10] * -0.020168554 + n_1[11] * -0.45555001 + -1.9828625);
	n_2[1] = softplusActivation(n_1[0] * 3.5508893 + n_1[1] * -0.41148978 + n_1[2] * 0.30322537 + n_1[3] * -2.2569249 + n_1[4] * 0.45080444 + n_1[5] * -0.9459166 + n_1[6] * -1.3282183 + n_1[7] * -0.70690227 + n_1[8] * -1.3092971 + n_1[9] * 0.36331737 + n_1[10] * -0.37892786 + n_1[11] * -1.1757908 + -0.016213564);
	n_2[2] = softplusActivation(n_1[0] * 1.1646281 + n_1[1] * -1.0174012 + n_1[2] * -24.460766 + n_1[3] * -3.8606105 + n_1[4] * -0.101598024 + n_1[5] * 0.083356634 + n_1[6] * -2.0392191 + n_1[7] * 0.4329104 + n_1[8] * -10.7885685 + n_1[9] * 1.0119374 + n_1[10] * -0.2022951 + n_1[11] * -0.46426696 + -0.6245407);
	n_2[3] = softplusActivation(n_1[0] * 0.983934 + n_1[1] * -1.2968122 + n_1[2] * -0.02096855 + n_1[3] * -0.6721429 + n_1[4] * -0.24614738 + n_1[5] * -0.71587646 + n_1[6] * -1.1995891 + n_1[7] * 2.1614573 + n_1[8] * 0.19920677 + n_1[9] * 0.39059076 + n_1[10] * -0.47315055 + n_1[11] * -3.9203494 + -0.35426134);
	n_2[4] = softplusActivation(n_1[0] * -0.08433474 + n_1[1] * 1.7030731 + n_1[2] * -0.036362328 + n_1[3] * 3.5927436 + n_1[4] * -3.9728684 + n_1[5] * -2.954025 + n_1[6] * -0.94180346 + n_1[7] * 0.51406235 + n_1[8] * -2.1369357 + n_1[9] * -0.17270172 + n_1[10] * -2.6496468 + n_1[11] * -6.392213 + -0.63721085);
	n_2[5] = softplusActivation(n_1[0] * 2.6062207 + n_1[1] * 0.11036918 + n_1[2] * 0.28608352 + n_1[3] * 0.35081646 + n_1[4] * -0.5653267 + n_1[5] * -0.60810107 + n_1[6] * -1.6094788 + n_1[7] * -1.5880874 + n_1[8] * 0.38880238 + n_1[9] * 0.13360791 + n_1[10] * -0.25337577 + n_1[11] * 0.97495717 + -2.3289194);
	n_2[6] = softplusActivation(n_1[0] * -16.10413 + n_1[1] * -0.27874848 + n_1[2] * -50.654766 + n_1[3] * -0.34673247 + n_1[4] * -0.29119155 + n_1[5] * 0.041391235 + n_1[6] * 1.2743104 + n_1[7] * 0.06181712 + n_1[8] * 0.83987707 + n_1[9] * 4.70435 + n_1[10] * 0.36897254 + n_1[11] * -0.054621205 + -1.8508223);
	n_2[7] = softplusActivation(n_1[0] * -2.5591936 + n_1[1] * -2.812306 + n_1[2] * 0.5375688 + n_1[3] * 0.70070213 + n_1[4] * 0.94221514 + n_1[5] * 3.8251052 + n_1[6] * 2.6433346 + n_1[7] * -0.3209908 + n_1[8] * -1.0233734 + n_1[9] * -40.18829 + n_1[10] * -1.2399704 + n_1[11] * -0.6113449 + -4.1058364);
	n_2[8] = softplusActivation(n_1[0] * 1.6638274 + n_1[1] * -3.098195 + n_1[2] * 0.014866623 + n_1[3] * -0.62260157 + n_1[4] * 0.9573909 + n_1[5] * -0.2658974 + n_1[6] * 0.045695048 + n_1[7] * 0.38918197 + n_1[8] * -0.36784044 + n_1[9] * 1.6611104 + n_1[10] * -0.85063004 + n_1[11] * -0.6366247 + -0.5545722);
	n_2[9] = softplusActivation(n_1[0] * 0.22139075 + n_1[1] * 0.9503512 + n_1[2] * -2.150665 + n_1[3] * 0.41344982 + n_1[4] * 0.61165553 + n_1[5] * -0.517665 + n_1[6] * -3.0010245 + n_1[7] * -0.0113169635 + n_1[8] * 0.62274325 + n_1[9] * -3.1184623 + n_1[10] * -0.5991326 + n_1[11] * 0.012693656 + -2.2097313);
	n_2[10] = softplusActivation(n_1[0] * -0.82766277 + n_1[1] * 0.9903294 + n_1[2] * -24.04464 + n_1[3] * 0.36433554 + n_1[4] * 0.501266 + n_1[5] * -0.49812096 + n_1[6] * -2.7432864 + n_1[7] * 0.011282657 + n_1[8] * -4.329057 + n_1[9] * -2.976238 + n_1[10] * -0.5630841 + n_1[11] * -0.032604598 + 1.5284678);
	n_2[11] = softplusActivation(n_1[0] * -1.4528275 + n_1[1] * 0.0510645 + n_1[2] * -1.5755064 + n_1[3] * -0.17802095 + n_1[4] * 0.7499086 + n_1[5] * -0.6046326 + n_1[6] * 0.15607545 + n_1[7] * 0.22094099 + n_1[8] * -1.038011 + n_1[9] * -1.4619735 + n_1[10] * 0.036541212 + n_1[11] * -0.46545812 + 1.5597595);
	n_3[0] = n_2[0] * -2.2055776 + n_2[1] * 0.0006287587 + n_2[2] * 0.751252 + n_2[3] * -0.25748882 + n_2[4] * 0.0037271376 + n_2[5] * -0.0012403494 + n_2[6] * -0.0011604413 + n_2[7] * -0.001597834 + n_2[8] * 0.07311169 + n_2[9] * 0.5031375 + n_2[10] * -0.5436491 + n_2[11] * 0.057898466 + 1.0002586;
	n_3[1] = n_2[0] * 0.048292495 + n_2[1] * 0.8438262 + n_2[2] * 0.011374571 + n_2[3] * -0.06942878 + n_2[4] * -0.26988438 + n_2[5] * 0.3015162 + n_2[6] * 0.037003998 + n_2[7] * -0.03871959 + n_2[8] * 0.26985425 + n_2[9] * 0.019907728 + n_2[10] * -0.080720805 + n_2[11] * 0.18341282 + -0.061368816;
	n_3[2] = n_2[0] * -6.9242174e-06 + n_2[1] * -2.422646e-06 + n_2[2] * -1.7244757e-06 + n_2[3] * 1.657952e-05 + n_2[4] * -1.293184e-05 + n_2[5] * -4.4099626e-05 + n_2[6] * 0.00020657777 + n_2[7] * -0.00011695847 + n_2[8] * -3.4653565e-06 + n_2[9] * 0.00031939652 + n_2[10] * -0.00022539934 + n_2[11] * -2.6978363e-05 + 5.5287755e-06;
	n_3[3] = n_2[0] * -1.4902292 + n_2[1] * -1.8855245 + n_2[2] * -21.037287 + n_2[3] * -1.7990141 + n_2[4] * -5.1507215 + n_2[5] * -51.352352 + n_2[6] * -7.4764857 + n_2[7] * -0.5165209 + n_2[8] * -10.428566 + n_2[9] * -0.9556056 + n_2[10] * -0.39862505 + n_2[11] * 0.21175666 + -3.9034646;
	n_3[4] = n_2[0] * -9.380778 + n_2[1] * -6.6302195 + n_2[2] * 1.5010784 + n_2[3] * 1.1949453 + n_2[4] * -14.914186 + n_2[5] * 5.819829 + n_2[6] * -0.11169912 + n_2[7] * -1.890843 + n_2[8] * -79.237274 + n_2[9] * 0.017835652 + n_2[10] * 0.010680344 + n_2[11] * -0.09361498 + -1.404226;
	n_3[5] = n_2[0] * -15.091814 + n_2[1] * -207.59052 + n_2[2] * 27.297535 + n_2[3] * -143.98557 + n_2[4] * -19.857454 + n_2[5] * 9.284048 + n_2[6] * 0.37022233 + n_2[7] * -3.6109376 + n_2[8] * -18.872278 + n_2[9] * -0.042760473 + n_2[10] * 0.04039806 + n_2[11] * -0.009267084 + -1.6429832;
}

