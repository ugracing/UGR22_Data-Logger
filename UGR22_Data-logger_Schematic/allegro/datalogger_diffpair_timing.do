
################################################################################
# Diff Pair RULES                                                              #
################################################################################

################################################################################
# rule assignments for class diff pair                                         #
################################################################################
define (drcv_group _DRgrp_D+
 (drcv U13-71 U3-6 )
)
define (drcv_group '_DRgrp_D-'
 (drcv U13-70 U3-7 )
)
define (drcv_groupset '_DRgrpset_D+_D-' _DRgrp_D+ '_DRgrp_D-')
#forget drcv_groupset '_DRgrpset_D+_D-'
define (drcv_group _DRgrp_N883057
 (drcv U3-2 J25-2 )
)
define (drcv_group _DRgrp_N883071
 (drcv U3-3 J25-3 )
)
define (drcv_groupset _DRgrpset_N883057_N883071 _DRgrp_N883057 _DRgrp_N883071)
#forget drcv_groupset _DRgrpset_N883057_N883071

################################################################################
# rule assignments for group diff pair                                         #
################################################################################

# end of do file