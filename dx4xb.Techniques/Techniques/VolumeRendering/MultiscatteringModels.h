float softplusActivation(float x) { return log(1 + exp(x)); }

void lenModel(float n_0[3], out float n_5[2]) {
	float t_1[6];
	float t_2[6];

	t_1[0] = softplusActivation(n_0[0] * 0.57789564 + n_0[1] * -0.96967626 + n_0[2] * -1.0306507 + 1.0596187);
	t_1[1] = softplusActivation(n_0[0] * -0.39895195 + n_0[1] * -0.111746654 + n_0[2] * 0.8616168 + 0.15618202);
	t_1[2] = softplusActivation(n_0[0] * -0.4730365 + n_0[1] * -0.26211852 + n_0[2] * 0.27060506 + 0.75600046);
	t_1[3] = softplusActivation(n_0[0] * -0.3083972 + n_0[1] * -0.08515844 + n_0[2] * 0.4574682 + 1.214174);
	t_1[4] = softplusActivation(n_0[0] * 0.40055776 + n_0[1] * -0.03865928 + n_0[2] * 0.1796862 + -0.49998996);
	t_1[5] = softplusActivation(n_0[0] * 0.5311652 + n_0[1] * 0.2772712 + n_0[2] * -0.87597525 + -0.1489665);
	t_2[0] = softplusActivation(t_1[0] * 0.00044242782 + t_1[1] * -1.2634984 + t_1[2] * 0.8408821 + t_1[3] * -1.1341012 + t_1[4] * 0.31271237 + t_1[5] * 0.43974158 + 0.97988427);
	t_2[1] = softplusActivation(t_1[0] * 0.37955645 + t_1[1] * -0.5308481 + t_1[2] * -0.6907023 + t_1[3] * 0.7943273 + t_1[4] * -0.27448702 + t_1[5] * -0.21142438 + 0.58490855);
	t_2[2] = softplusActivation(t_1[0] * -0.538829 + t_1[1] * -0.119841 + t_1[2] * 2.3909922 + t_1[3] * 1.8592691 + t_1[4] * -0.05205027 + t_1[5] * 0.65840894 + 0.33053064);
	t_2[3] = softplusActivation(t_1[0] * 0.5543533 + t_1[1] * 0.048358805 + t_1[2] * -0.13365631 + t_1[3] * -0.5423104 + t_1[4] * -0.2517838 + t_1[5] * -0.36510321 + -0.79389477);
	t_2[4] = softplusActivation(t_1[0] * -0.123385206 + t_1[1] * 0.13422525 + t_1[2] * -0.24875425 + t_1[3] * -0.59919834 + t_1[4] * 0.20503607 + t_1[5] * -0.2905361 + 0.60001934);
	t_2[5] = softplusActivation(t_1[0] * 0.20990759 + t_1[1] * -0.7667715 + t_1[2] * -0.59041774 + t_1[3] * 0.7887424 + t_1[4] * 0.42831707 + t_1[5] * 0.42810676 + -0.9770538);
	t_1[0] = softplusActivation(t_2[0] * 0.67584443 + t_2[1] * 0.66350186 + t_2[2] * 0.22419377 + t_2[3] * -0.19988456 + t_2[4] * 0.67953795 + t_2[5] * 0.4376294 + 0.22602105);
	t_1[1] = softplusActivation(t_2[0] * 0.72893476 + t_2[1] * 5.369519 + t_2[2] * -4.095704 + t_2[3] * -0.06699239 + t_2[4] * -0.6026892 + t_2[5] * -0.33123946 + 6.5759397);
	t_1[2] = softplusActivation(t_2[0] * 0.38535333 + t_2[1] * -0.32967362 + t_2[2] * -3.0726736 + t_2[3] * 0.4475086 + t_2[4] * -3.4267387 + t_2[5] * -0.13681257 + 1.3901032);
	t_1[3] = softplusActivation(t_2[0] * -0.17690015 + t_2[1] * 0.016185066 + t_2[2] * -4.561383 + t_2[3] * 0.9959099 + t_2[4] * -0.8699933 + t_2[5] * 0.48825532 + 2.4230435);
	t_1[4] = softplusActivation(t_2[0] * -0.5002485 + t_2[1] * -0.35107356 + t_2[2] * 0.7435274 + t_2[3] * -0.6533266 + t_2[4] * -0.23578691 + t_2[5] * 0.51515836 + -0.61564064);
	t_1[5] = softplusActivation(t_2[0] * 0.72126466 + t_2[1] * -0.161914 + t_2[2] * 1.0729828 + t_2[3] * -0.4225838 + t_2[4] * -1.2060237 + t_2[5] * -0.09456201 + 0.3006834);
	t_2[0] = softplusActivation(t_1[0] * 0.0948105 + t_1[1] * -0.093529865 + t_1[2] * -2.2946165 + t_1[3] * -1.3065977 + t_1[4] * 0.36123317 + t_1[5] * 0.8090586 + -0.39399302);
	t_2[1] = softplusActivation(t_1[0] * -0.37371704 + t_1[1] * 0.44464526 + t_1[2] * -1.0358881 + t_1[3] * -0.3315798 + t_1[4] * 0.06765868 + t_1[5] * 1.2069967 + -0.07085896);
	t_2[2] = softplusActivation(t_1[0] * -0.5374587 + t_1[1] * 0.13266285 + t_1[2] * 2.1382554 + t_1[3] * 0.13977619 + t_1[4] * -0.00030811416 + t_1[5] * 0.9916173 + -0.97217894);
	t_2[3] = softplusActivation(t_1[0] * -0.11223587 + t_1[1] * -0.9590203 + t_1[2] * 1.3771987 + t_1[3] * 1.2633772 + t_1[4] * -0.3521504 + t_1[5] * 0.084432475 + 0.7819139);
	t_2[4] = softplusActivation(t_1[0] * -0.69299704 + t_1[1] * -0.11189661 + t_1[2] * 1.8875507 + t_1[3] * 1.1264659 + t_1[4] * 0.3490606 + t_1[5] * 0.88045347 + -0.8549816);
	t_2[5] = softplusActivation(t_1[0] * 0.60440016 + t_1[1] * 0.3735578 + t_1[2] * 5.343762 + t_1[3] * -1.4122556 + t_1[4] * 0.32099685 + t_1[5] * -0.32013872 + 0.5893578);
	n_5[0] = t_2[0] * -0.734935 + t_2[1] * 0.2969631 + t_2[2] * 0.5505526 + t_2[3] * -0.15928733 + t_2[4] * -0.10986916 + t_2[5] * 0.08471288 + 0.93591;
	n_5[1] = t_2[0] * -0.36777073 + t_2[1] * -1.9012893 + t_2[2] * -1.5743685 + t_2[3] * -0.06691273 + t_2[4] * -1.2729967 + t_2[5] * 1.315749 + 1.2637364;
}

void pathModel(float n_0[6], out float n_5[6]) {
	float t_1[6];
	float t_2[6];
	t_1[0] = softplusActivation(n_0[0] * -0.017949123 + n_0[1] * -0.6442109 + n_0[2] * 3.213 + n_0[3] * -1.0757365 + n_0[4] * 0.00017855092 + n_0[5] * 0.06214093 + -0.6205637);
	t_1[1] = softplusActivation(n_0[0] * -0.004222867 + n_0[1] * 3.7662127 + n_0[2] * 0.93899333 + n_0[3] * -1.5603656 + n_0[4] * 0.00067858427 + n_0[5] * 0.009371768 + 1.6380408);
	t_1[2] = softplusActivation(n_0[0] * -0.0005906069 + n_0[1] * 6.0041323 + n_0[2] * -1.9172125 + n_0[3] * 0.7661247 + n_0[4] * 0.0008374448 + n_0[5] * 0.08113002 + 6.400009);
	t_1[3] = softplusActivation(n_0[0] * -0.0042575533 + n_0[1] * -0.46255785 + n_0[2] * -13.699913 + n_0[3] * -0.21955352 + n_0[4] * -9.828843e-05 + n_0[5] * -0.011528204 + 4.2798734);
	t_1[4] = softplusActivation(n_0[0] * -0.02533899 + n_0[1] * -0.32006073 + n_0[2] * 4.5826783 + n_0[3] * 2.458025 + n_0[4] * 0.0003560429 + n_0[5] * 0.0748664 + 1.8762898);
	t_1[5] = softplusActivation(n_0[0] * -0.022125775 + n_0[1] * -3.678189 + n_0[2] * 4.0094056 + n_0[3] * 2.440443 + n_0[4] * 0.00063302374 + n_0[5] * 0.0496811 + 5.0792966);
	t_2[0] = softplusActivation(t_1[0] * 0.15602326 + t_1[1] * -0.14398889 + t_1[2] * -0.4385112 + t_1[3] * 1.7847224 + t_1[4] * 5.590357 + t_1[5] * -7.249054 + 0.1308024);
	t_2[1] = softplusActivation(t_1[0] * 1.3226305 + t_1[1] * -1.6199534 + t_1[2] * 0.96517104 + t_1[3] * 0.97196215 + t_1[4] * -1.3324488 + t_1[5] * 0.5897824 + -4.0974913);
	t_2[2] = softplusActivation(t_1[0] * -1.1448756 + t_1[1] * 0.4839611 + t_1[2] * -0.38459036 + t_1[3] * 0.19780582 + t_1[4] * 3.5946572 + t_1[5] * -3.7316628 + 1.4761901);
	t_2[3] = softplusActivation(t_1[0] * -0.016682653 + t_1[1] * -0.27815595 + t_1[2] * -0.028760377 + t_1[3] * 1.0538613 + t_1[4] * 0.59766984 + t_1[5] * -0.81184644 + 2.8207083);
	t_2[4] = softplusActivation(t_1[0] * -0.7513338 + t_1[1] * 0.31336227 + t_1[2] * -0.052348807 + t_1[3] * 0.941844 + t_1[4] * -0.013640403 + t_1[5] * 0.53510255 + -4.0574903);
	t_2[5] = softplusActivation(t_1[0] * -0.22344841 + t_1[1] * -0.25496253 + t_1[2] * 0.03999354 + t_1[3] * -30.04752 + t_1[4] * 0.44383273 + t_1[5] * -0.26517916 + 5.366396);
	t_1[0] = softplusActivation(t_2[0] * -3.0183184 + t_2[1] * 0.69209385 + t_2[2] * 0.43844664 + t_2[3] * 0.7725622 + t_2[4] * -3.5327547 + t_2[5] * -0.7274119 + -0.68007046);
	t_1[1] = softplusActivation(t_2[0] * -0.11423675 + t_2[1] * -0.02782498 + t_2[2] * 0.92147666 + t_2[3] * 0.18665218 + t_2[4] * -1.3575885 + t_2[5] * -0.24278975 + 1.5475862);
	t_1[2] = softplusActivation(t_2[0] * -2.0454624 + t_2[1] * -1.3993104 + t_2[2] * -3.7429013 + t_2[3] * -1.3338817 + t_2[4] * -1.5559304 + t_2[5] * 0.0007374244 + -0.16098751);
	t_1[3] = softplusActivation(t_2[0] * -4.9359 + t_2[1] * -0.45022276 + t_2[2] * -3.8212826 + t_2[3] * -0.20617026 + t_2[4] * 0.39759275 + t_2[5] * 0.5690424 + -2.7297103);
	t_1[4] = softplusActivation(t_2[0] * -1.2459557 + t_2[1] * 0.5027867 + t_2[2] * 2.7313333 + t_2[3] * 0.054335576 + t_2[4] * -2.0433407 + t_2[5] * -0.66605437 + -1.1484785);
	t_1[5] = softplusActivation(t_2[0] * -3.6019754 + t_2[1] * 0.7402415 + t_2[2] * -0.66030914 + t_2[3] * 0.38176656 + t_2[4] * 0.23192269 + t_2[5] * 0.16101685 + -3.009221);
	t_2[0] = softplusActivation(t_1[0] * 1.5958327 + t_1[1] * -1.0725702 + t_1[2] * -0.07181342 + t_1[3] * -4.7023387 + t_1[4] * 1.2863337 + t_1[5] * 0.15011753 + -0.97994894);
	t_2[1] = softplusActivation(t_1[0] * 5.05151 + t_1[1] * -1.1656743 + t_1[2] * 1.895798 + t_1[3] * 0.13388169 + t_1[4] * -6.448373 + t_1[5] * -5.122183 + 1.6592312);
	t_2[2] = softplusActivation(t_1[0] * -2.2878976 + t_1[1] * -6.537621 + t_1[2] * 8.992946 + t_1[3] * -0.001876841 + t_1[4] * -22.677755 + t_1[5] * 0.011322948 + -0.11784841);
	t_2[3] = softplusActivation(t_1[0] * 0.8331082 + t_1[1] * 0.05133309 + t_1[2] * -0.035025902 + t_1[3] * -1.5193884 + t_1[4] * 0.6211994 + t_1[5] * -0.18183362 + -1.5565299);
	t_2[4] = softplusActivation(t_1[0] * 0.8715139 + t_1[1] * 0.22868231 + t_1[2] * -0.03204425 + t_1[3] * -1.546553 + t_1[4] * 0.9710473 + t_1[5] * -0.3617829 + -0.80527884);
	t_2[5] = softplusActivation(t_1[0] * -0.84714615 + t_1[1] * -0.5753768 + t_1[2] * 0.9561754 + t_1[3] * -0.1943496 + t_1[4] * 2.0143898 + t_1[5] * -1.156723 + 0.9367227);
	n_5[0] = t_2[0] * -0.050380476 + t_2[1] * -0.0007678078 + t_2[2] * -3.132679 + t_2[3] * 0.1912767 + t_2[4] * -0.05582087 + t_2[5] * 0.0017601933 + 0.9906731;
	n_5[1] = t_2[0] * 0.8279946 + t_2[1] * -0.25208473 + t_2[2] * -0.66289514 + t_2[3] * -3.1424015 + t_2[4] * -0.037517782 + t_2[5] * 0.14680699 + 0.61847407;
	n_5[2] = t_2[0] * -0.013407477 + t_2[1] * -0.005012992 + t_2[2] * 0.0013173942 + t_2[3] * -0.11344983 + t_2[4] * 0.047029305 + t_2[5] * -0.011353133 + 0.012321483;
	n_5[3] = t_2[0] * 39.203827 + t_2[1] * 6.305907 + t_2[2] * -7.137204 + t_2[3] * -8.021676 + t_2[4] * -22.202002 + t_2[5] * 4.3610196 + -13.235121;
	n_5[4] = t_2[0] * -16.69766 + t_2[1] * 11.497937 + t_2[2] * -2.277504 + t_2[3] * 44.144794 + t_2[4] * -10.553386 + t_2[5] * -19.5203 + 2.6012592;
	n_5[5] = t_2[0] * -75.59726 + t_2[1] * 8.826765 + t_2[2] * 1.0285189 + t_2[3] * -20.491722 + t_2[4] * 25.6232 + t_2[5] * -14.533028 + -0.5661598;
}

