use pr21::define_io_ctx;

define_io_ctx!(struct Context {
    // input variables
    g_in_x => in_x: f32,
    g_in_y => in_y: f32,
    g_in_z => in_z: f32,

    // frame counter
    g_frame => frame: i32,

    // body status
    g_waist_x => waist_x: f32,
    g_waist_y => waist_y: f32,
    g_waist_z => waist_z: f32,

    g_head_x => head_x: f32,
    g_head_y => head_y: f32,
    g_head_z => head_z: f32,

    // arms, legs and hand status
    g_arm_l0 => arm_l0: f32,
    g_arm_l1 => arm_l1: f32,
    g_arm_l2 => arm_l2: f32,
    g_arm_l3 => arm_l3: f32,
    g_arm_l4 => arm_l4: f32,

    g_arm_r0 => arm_r0: f32,
    g_arm_r1 => arm_r1: f32,
    g_arm_r2 => arm_r2: f32,
    g_arm_r3 => arm_r3: f32,
    g_arm_r4 => arm_r4: f32,

    g_leg_l0 => leg_l0: f32,
    g_leg_l1 => leg_l1: f32,
    g_leg_l2 => leg_l2: f32,
    g_leg_l3 => leg_l3: f32,

    g_leg_r0 => leg_r0: f32,
    g_leg_r1 => leg_r1: f32,
    g_leg_r2 => leg_r2: f32,
    g_leg_r3 => leg_r3: f32,

    // helper variables
    g_i0 => i0: i32,
    g_i1 => i1: i32,
    g_i2 => i2: i32,
    g_i3 => i3: i32,
    g_i4 => i4: i32,
    g_i5 => i5: i32,

    g_f0 => f0: f32,
    g_f1 => f1: f32,
    g_f2 => f2: f32,
    g_f3 => f3: f32,
    g_f4 => f4: f32,
    g_f5 => f5: f32,

    // config parameters
    g_cfg_i0 => cfg_i0: i32,
    g_cfg_i1 => cfg_i1: i32,
    g_cfg_i2 => cfg_i2: i32,
    g_cfg_i3 => cfg_i3: i32,

    g_cfg_f0 => cfg_f0: f32,
    g_cfg_f1 => cfg_f1: f32,
    g_cfg_f2 => cfg_f2: f32,
    g_cfg_f3 => cfg_f3: f32
});

#[cfg(test)]
#[test]
fn test_ctx_size() {
    dbg!(std::mem::size_of::<Context>());
    eprintln!("metadata = {:?}", <Context as IOContext>::metadata());
}
