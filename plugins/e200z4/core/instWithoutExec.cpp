/**
 * generated by HARMLESS : 'Hardware ARchitecture Modeling Language
 *                          for Embedded Software Simulation'
 * model : e200z4
 *
 */

#include "instruction.h" //first for precompiled headers
#include <iostream>
#include "macro.h"
#include "arch.h"
#include "utils.h"

using namespace std;

void e200z4_instruction::release() {
	m_user --;
	SEGFAULTIF(m_user < 0);
	if(!m_user) delete this;
}

void e200z4_instructionStall::execute(arch *_arch){}

// instructions without any behavior...

void e200z4_CR0_crfd_crfs_mcrfs::execute(arch *_arch){
	cerr << "instruction e200z4_CR0_crfd_crfs_mcrfs have no behavior defined" << endl;
}
void e200z4_CR0_crfd_fcmpo_fra_frb::execute(arch *_arch){
	cerr << "instruction e200z4_CR0_crfd_fcmpo_fra_frb have no behavior defined" << endl;
}
void e200z4_CR0_crfd_fcmpu_fra_frb::execute(arch *_arch){
	cerr << "instruction e200z4_CR0_crfd_fcmpu_fra_frb have no behavior defined" << endl;
}
void e200z4_CR0_crfd_imm_mtfsfi_noStatus::execute(arch *_arch){
	cerr << "instruction e200z4_CR0_crfd_imm_mtfsfi_noStatus have no behavior defined" << endl;
}
void e200z4_CR0_crfd_imm_mtfsfi_useStatus::execute(arch *_arch){
	cerr << "instruction e200z4_CR0_crfd_imm_mtfsfi_useStatus have no behavior defined" << endl;
}
void e200z4_CR1_crfd_crfs_mcrfs::execute(arch *_arch){
	cerr << "instruction e200z4_CR1_crfd_crfs_mcrfs have no behavior defined" << endl;
}
void e200z4_CR1_crfd_fcmpo_fra_frb::execute(arch *_arch){
	cerr << "instruction e200z4_CR1_crfd_fcmpo_fra_frb have no behavior defined" << endl;
}
void e200z4_CR1_crfd_fcmpu_fra_frb::execute(arch *_arch){
	cerr << "instruction e200z4_CR1_crfd_fcmpu_fra_frb have no behavior defined" << endl;
}
void e200z4_CR1_crfd_imm_mtfsfi_noStatus::execute(arch *_arch){
	cerr << "instruction e200z4_CR1_crfd_imm_mtfsfi_noStatus have no behavior defined" << endl;
}
void e200z4_CR1_crfd_imm_mtfsfi_useStatus::execute(arch *_arch){
	cerr << "instruction e200z4_CR1_crfd_imm_mtfsfi_useStatus have no behavior defined" << endl;
}
void e200z4_CR2_crfd_crfs_mcrfs::execute(arch *_arch){
	cerr << "instruction e200z4_CR2_crfd_crfs_mcrfs have no behavior defined" << endl;
}
void e200z4_CR2_crfd_fcmpo_fra_frb::execute(arch *_arch){
	cerr << "instruction e200z4_CR2_crfd_fcmpo_fra_frb have no behavior defined" << endl;
}
void e200z4_CR2_crfd_fcmpu_fra_frb::execute(arch *_arch){
	cerr << "instruction e200z4_CR2_crfd_fcmpu_fra_frb have no behavior defined" << endl;
}
void e200z4_CR2_crfd_imm_mtfsfi_noStatus::execute(arch *_arch){
	cerr << "instruction e200z4_CR2_crfd_imm_mtfsfi_noStatus have no behavior defined" << endl;
}
void e200z4_CR2_crfd_imm_mtfsfi_useStatus::execute(arch *_arch){
	cerr << "instruction e200z4_CR2_crfd_imm_mtfsfi_useStatus have no behavior defined" << endl;
}
void e200z4_CR3_crfd_crfs_mcrfs::execute(arch *_arch){
	cerr << "instruction e200z4_CR3_crfd_crfs_mcrfs have no behavior defined" << endl;
}
void e200z4_CR3_crfd_fcmpo_fra_frb::execute(arch *_arch){
	cerr << "instruction e200z4_CR3_crfd_fcmpo_fra_frb have no behavior defined" << endl;
}
void e200z4_CR3_crfd_fcmpu_fra_frb::execute(arch *_arch){
	cerr << "instruction e200z4_CR3_crfd_fcmpu_fra_frb have no behavior defined" << endl;
}
void e200z4_CR3_crfd_imm_mtfsfi_noStatus::execute(arch *_arch){
	cerr << "instruction e200z4_CR3_crfd_imm_mtfsfi_noStatus have no behavior defined" << endl;
}
void e200z4_CR3_crfd_imm_mtfsfi_useStatus::execute(arch *_arch){
	cerr << "instruction e200z4_CR3_crfd_imm_mtfsfi_useStatus have no behavior defined" << endl;
}
void e200z4_CR4_crfd_crfs_mcrfs::execute(arch *_arch){
	cerr << "instruction e200z4_CR4_crfd_crfs_mcrfs have no behavior defined" << endl;
}
void e200z4_CR4_crfd_fcmpo_fra_frb::execute(arch *_arch){
	cerr << "instruction e200z4_CR4_crfd_fcmpo_fra_frb have no behavior defined" << endl;
}
void e200z4_CR4_crfd_fcmpu_fra_frb::execute(arch *_arch){
	cerr << "instruction e200z4_CR4_crfd_fcmpu_fra_frb have no behavior defined" << endl;
}
void e200z4_CR4_crfd_imm_mtfsfi_noStatus::execute(arch *_arch){
	cerr << "instruction e200z4_CR4_crfd_imm_mtfsfi_noStatus have no behavior defined" << endl;
}
void e200z4_CR4_crfd_imm_mtfsfi_useStatus::execute(arch *_arch){
	cerr << "instruction e200z4_CR4_crfd_imm_mtfsfi_useStatus have no behavior defined" << endl;
}
void e200z4_CR5_crfd_crfs_mcrfs::execute(arch *_arch){
	cerr << "instruction e200z4_CR5_crfd_crfs_mcrfs have no behavior defined" << endl;
}
void e200z4_CR5_crfd_fcmpo_fra_frb::execute(arch *_arch){
	cerr << "instruction e200z4_CR5_crfd_fcmpo_fra_frb have no behavior defined" << endl;
}
void e200z4_CR5_crfd_fcmpu_fra_frb::execute(arch *_arch){
	cerr << "instruction e200z4_CR5_crfd_fcmpu_fra_frb have no behavior defined" << endl;
}
void e200z4_CR5_crfd_imm_mtfsfi_noStatus::execute(arch *_arch){
	cerr << "instruction e200z4_CR5_crfd_imm_mtfsfi_noStatus have no behavior defined" << endl;
}
void e200z4_CR5_crfd_imm_mtfsfi_useStatus::execute(arch *_arch){
	cerr << "instruction e200z4_CR5_crfd_imm_mtfsfi_useStatus have no behavior defined" << endl;
}
void e200z4_CR6_crfd_crfs_mcrfs::execute(arch *_arch){
	cerr << "instruction e200z4_CR6_crfd_crfs_mcrfs have no behavior defined" << endl;
}
void e200z4_CR6_crfd_fcmpo_fra_frb::execute(arch *_arch){
	cerr << "instruction e200z4_CR6_crfd_fcmpo_fra_frb have no behavior defined" << endl;
}
void e200z4_CR6_crfd_fcmpu_fra_frb::execute(arch *_arch){
	cerr << "instruction e200z4_CR6_crfd_fcmpu_fra_frb have no behavior defined" << endl;
}
void e200z4_CR6_crfd_imm_mtfsfi_noStatus::execute(arch *_arch){
	cerr << "instruction e200z4_CR6_crfd_imm_mtfsfi_noStatus have no behavior defined" << endl;
}
void e200z4_CR6_crfd_imm_mtfsfi_useStatus::execute(arch *_arch){
	cerr << "instruction e200z4_CR6_crfd_imm_mtfsfi_useStatus have no behavior defined" << endl;
}
void e200z4_CR7_crfd_crfs_mcrfs::execute(arch *_arch){
	cerr << "instruction e200z4_CR7_crfd_crfs_mcrfs have no behavior defined" << endl;
}
void e200z4_CR7_crfd_fcmpo_fra_frb::execute(arch *_arch){
	cerr << "instruction e200z4_CR7_crfd_fcmpo_fra_frb have no behavior defined" << endl;
}
void e200z4_CR7_crfd_fcmpu_fra_frb::execute(arch *_arch){
	cerr << "instruction e200z4_CR7_crfd_fcmpu_fra_frb have no behavior defined" << endl;
}
void e200z4_CR7_crfd_imm_mtfsfi_noStatus::execute(arch *_arch){
	cerr << "instruction e200z4_CR7_crfd_imm_mtfsfi_noStatus have no behavior defined" << endl;
}
void e200z4_CR7_crfd_imm_mtfsfi_useStatus::execute(arch *_arch){
	cerr << "instruction e200z4_CR7_crfd_imm_mtfsfi_useStatus have no behavior defined" << endl;
}
void e200z4_algebraic_load_notu_r_ra_rb_rd_word_x::execute(arch *_arch){
	cerr << "instruction e200z4_algebraic_load_notu_r_ra_rb_rd_word_x have no behavior defined" << endl;
}
void e200z4_c_notu_ra_rb_rs_store_useStatus_word_x::execute(arch *_arch){
	cerr << "instruction e200z4_c_notu_ra_rb_rs_store_useStatus_word_x have no behavior defined" << endl;
}
void e200z4_crbd_mtfsb0_noStatus::execute(arch *_arch){
	cerr << "instruction e200z4_crbd_mtfsb0_noStatus have no behavior defined" << endl;
}
void e200z4_crbd_mtfsb0_useStatus::execute(arch *_arch){
	cerr << "instruction e200z4_crbd_mtfsb0_useStatus have no behavior defined" << endl;
}
void e200z4_crbd_mtfsb1_noStatus::execute(arch *_arch){
	cerr << "instruction e200z4_crbd_mtfsb1_noStatus have no behavior defined" << endl;
}
void e200z4_crbd_mtfsb1_useStatus::execute(arch *_arch){
	cerr << "instruction e200z4_crbd_mtfsb1_useStatus have no behavior defined" << endl;
}
void e200z4_d_frd_fs_load_notu_ra::execute(arch *_arch){
	cerr << "instruction e200z4_d_frd_fs_load_notu_ra have no behavior defined" << endl;
}
void e200z4_d_frd_fs_load_ra_u::execute(arch *_arch){
	cerr << "instruction e200z4_d_frd_fs_load_ra_u have no behavior defined" << endl;
}
void e200z4_d_frs_fs_notu_ra_store::execute(arch *_arch){
	cerr << "instruction e200z4_d_frs_fs_notu_ra_store have no behavior defined" << endl;
}
void e200z4_d_frs_fs_ra_store_u::execute(arch *_arch){
	cerr << "instruction e200z4_d_frs_fs_ra_store_u have no behavior defined" << endl;
}
void e200z4_d_load_mw_notu_ra_rd::execute(arch *_arch){
	cerr << "instruction e200z4_d_load_mw_notu_ra_rd have no behavior defined" << endl;
}
void e200z4_dcba_ra_rb::execute(arch *_arch){
	cerr << "instruction e200z4_dcba_ra_rb have no behavior defined" << endl;
}
void e200z4_dcbf_ra_rb::execute(arch *_arch){
	cerr << "instruction e200z4_dcbf_ra_rb have no behavior defined" << endl;
}
void e200z4_dcbi_ra_rb::execute(arch *_arch){
	cerr << "instruction e200z4_dcbi_ra_rb have no behavior defined" << endl;
}
void e200z4_dcbst_ra_rb::execute(arch *_arch){
	cerr << "instruction e200z4_dcbst_ra_rb have no behavior defined" << endl;
}
void e200z4_dcbt_ra_rb::execute(arch *_arch){
	cerr << "instruction e200z4_dcbt_ra_rb have no behavior defined" << endl;
}
void e200z4_dcbtst_ra_rb::execute(arch *_arch){
	cerr << "instruction e200z4_dcbtst_ra_rb have no behavior defined" << endl;
}
void e200z4_dcbz_ra_rb::execute(arch *_arch){
	cerr << "instruction e200z4_dcbz_ra_rb have no behavior defined" << endl;
}
void e200z4_eciwx_ra_rb_rd::execute(arch *_arch){
	cerr << "instruction e200z4_eciwx_ra_rb_rd have no behavior defined" << endl;
}
void e200z4_ecowx_ra_rb_rs::execute(arch *_arch){
	cerr << "instruction e200z4_ecowx_ra_rb_rs have no behavior defined" << endl;
}
void e200z4_eieio::execute(arch *_arch){
	cerr << "instruction e200z4_eieio have no behavior defined" << endl;
}
void e200z4_fabs_frb_frd_noStatus::execute(arch *_arch){
	cerr << "instruction e200z4_fabs_frb_frd_noStatus have no behavior defined" << endl;
}
void e200z4_fabs_frb_frd_useStatus::execute(arch *_arch){
	cerr << "instruction e200z4_fabs_frb_frd_useStatus have no behavior defined" << endl;
}
void e200z4_fadd_fra_frb_frd_noStatus::execute(arch *_arch){
	cerr << "instruction e200z4_fadd_fra_frb_frd_noStatus have no behavior defined" << endl;
}
void e200z4_fadd_fra_frb_frd_useStatus::execute(arch *_arch){
	cerr << "instruction e200z4_fadd_fra_frb_frd_useStatus have no behavior defined" << endl;
}
void e200z4_fadds_fra_frb_frd_noStatus::execute(arch *_arch){
	cerr << "instruction e200z4_fadds_fra_frb_frd_noStatus have no behavior defined" << endl;
}
void e200z4_fadds_fra_frb_frd_useStatus::execute(arch *_arch){
	cerr << "instruction e200z4_fadds_fra_frb_frd_useStatus have no behavior defined" << endl;
}
void e200z4_fctiw_frb_frd_noStatus::execute(arch *_arch){
	cerr << "instruction e200z4_fctiw_frb_frd_noStatus have no behavior defined" << endl;
}
void e200z4_fctiw_frb_frd_useStatus::execute(arch *_arch){
	cerr << "instruction e200z4_fctiw_frb_frd_useStatus have no behavior defined" << endl;
}
void e200z4_fctiwz_frb_frd_noStatus::execute(arch *_arch){
	cerr << "instruction e200z4_fctiwz_frb_frd_noStatus have no behavior defined" << endl;
}
void e200z4_fctiwz_frb_frd_useStatus::execute(arch *_arch){
	cerr << "instruction e200z4_fctiwz_frb_frd_useStatus have no behavior defined" << endl;
}
void e200z4_fd_frd_load_notu_ra_rb_x::execute(arch *_arch){
	cerr << "instruction e200z4_fd_frd_load_notu_ra_rb_x have no behavior defined" << endl;
}
void e200z4_fd_frd_load_ra_rb_u_x::execute(arch *_arch){
	cerr << "instruction e200z4_fd_frd_load_ra_rb_u_x have no behavior defined" << endl;
}
void e200z4_fd_frs_notu_ra_rb_store_x::execute(arch *_arch){
	cerr << "instruction e200z4_fd_frs_notu_ra_rb_store_x have no behavior defined" << endl;
}
void e200z4_fd_frs_ra_rb_store_u_x::execute(arch *_arch){
	cerr << "instruction e200z4_fd_frs_ra_rb_store_u_x have no behavior defined" << endl;
}
void e200z4_fdiv_fra_frb_frd_noStatus::execute(arch *_arch){
	cerr << "instruction e200z4_fdiv_fra_frb_frd_noStatus have no behavior defined" << endl;
}
void e200z4_fdiv_fra_frb_frd_useStatus::execute(arch *_arch){
	cerr << "instruction e200z4_fdiv_fra_frb_frd_useStatus have no behavior defined" << endl;
}
void e200z4_fdivs_fra_frb_frd_noStatus::execute(arch *_arch){
	cerr << "instruction e200z4_fdivs_fra_frb_frd_noStatus have no behavior defined" << endl;
}
void e200z4_fdivs_fra_frb_frd_useStatus::execute(arch *_arch){
	cerr << "instruction e200z4_fdivs_fra_frb_frd_useStatus have no behavior defined" << endl;
}
void e200z4_fiw_frs_notu_ra_rb_store_x::execute(arch *_arch){
	cerr << "instruction e200z4_fiw_frs_notu_ra_rb_store_x have no behavior defined" << endl;
}
void e200z4_fm_frb_mtfsf_noStatus::execute(arch *_arch){
	cerr << "instruction e200z4_fm_frb_mtfsf_noStatus have no behavior defined" << endl;
}
void e200z4_fm_frb_mtfsf_useStatus::execute(arch *_arch){
	cerr << "instruction e200z4_fm_frb_mtfsf_useStatus have no behavior defined" << endl;
}
void e200z4_fmadd_fra_frb_frc_frd_noStatus::execute(arch *_arch){
	cerr << "instruction e200z4_fmadd_fra_frb_frc_frd_noStatus have no behavior defined" << endl;
}
void e200z4_fmadd_fra_frb_frc_frd_useStatus::execute(arch *_arch){
	cerr << "instruction e200z4_fmadd_fra_frb_frc_frd_useStatus have no behavior defined" << endl;
}
void e200z4_fmadds_fra_frb_frc_frd_noStatus::execute(arch *_arch){
	cerr << "instruction e200z4_fmadds_fra_frb_frc_frd_noStatus have no behavior defined" << endl;
}
void e200z4_fmadds_fra_frb_frc_frd_useStatus::execute(arch *_arch){
	cerr << "instruction e200z4_fmadds_fra_frb_frc_frd_useStatus have no behavior defined" << endl;
}
void e200z4_fmr_frb_frd_noStatus::execute(arch *_arch){
	cerr << "instruction e200z4_fmr_frb_frd_noStatus have no behavior defined" << endl;
}
void e200z4_fmr_frb_frd_useStatus::execute(arch *_arch){
	cerr << "instruction e200z4_fmr_frb_frd_useStatus have no behavior defined" << endl;
}
void e200z4_fmsub_fra_frb_frc_frd_noStatus::execute(arch *_arch){
	cerr << "instruction e200z4_fmsub_fra_frb_frc_frd_noStatus have no behavior defined" << endl;
}
void e200z4_fmsub_fra_frb_frc_frd_useStatus::execute(arch *_arch){
	cerr << "instruction e200z4_fmsub_fra_frb_frc_frd_useStatus have no behavior defined" << endl;
}
void e200z4_fmsubs_fra_frb_frc_frd_noStatus::execute(arch *_arch){
	cerr << "instruction e200z4_fmsubs_fra_frb_frc_frd_noStatus have no behavior defined" << endl;
}
void e200z4_fmsubs_fra_frb_frc_frd_useStatus::execute(arch *_arch){
	cerr << "instruction e200z4_fmsubs_fra_frb_frc_frd_useStatus have no behavior defined" << endl;
}
void e200z4_fmul_fra_frc_frd_noStatus::execute(arch *_arch){
	cerr << "instruction e200z4_fmul_fra_frc_frd_noStatus have no behavior defined" << endl;
}
void e200z4_fmul_fra_frc_frd_useStatus::execute(arch *_arch){
	cerr << "instruction e200z4_fmul_fra_frc_frd_useStatus have no behavior defined" << endl;
}
void e200z4_fmuls_fra_frc_frd_noStatus::execute(arch *_arch){
	cerr << "instruction e200z4_fmuls_fra_frc_frd_noStatus have no behavior defined" << endl;
}
void e200z4_fmuls_fra_frc_frd_useStatus::execute(arch *_arch){
	cerr << "instruction e200z4_fmuls_fra_frc_frd_useStatus have no behavior defined" << endl;
}
void e200z4_fnabs_frb_frd_noStatus::execute(arch *_arch){
	cerr << "instruction e200z4_fnabs_frb_frd_noStatus have no behavior defined" << endl;
}
void e200z4_fnabs_frb_frd_useStatus::execute(arch *_arch){
	cerr << "instruction e200z4_fnabs_frb_frd_useStatus have no behavior defined" << endl;
}
void e200z4_fneg_frb_frd_noStatus::execute(arch *_arch){
	cerr << "instruction e200z4_fneg_frb_frd_noStatus have no behavior defined" << endl;
}
void e200z4_fneg_frb_frd_useStatus::execute(arch *_arch){
	cerr << "instruction e200z4_fneg_frb_frd_useStatus have no behavior defined" << endl;
}
void e200z4_fnmadd_fra_frb_frc_frd_noStatus::execute(arch *_arch){
	cerr << "instruction e200z4_fnmadd_fra_frb_frc_frd_noStatus have no behavior defined" << endl;
}
void e200z4_fnmadd_fra_frb_frc_frd_useStatus::execute(arch *_arch){
	cerr << "instruction e200z4_fnmadd_fra_frb_frc_frd_useStatus have no behavior defined" << endl;
}
void e200z4_fnmadds_fra_frb_frc_frd_noStatus::execute(arch *_arch){
	cerr << "instruction e200z4_fnmadds_fra_frb_frc_frd_noStatus have no behavior defined" << endl;
}
void e200z4_fnmadds_fra_frb_frc_frd_useStatus::execute(arch *_arch){
	cerr << "instruction e200z4_fnmadds_fra_frb_frc_frd_useStatus have no behavior defined" << endl;
}
void e200z4_fnmsub_fra_frb_frc_frd_noStatus::execute(arch *_arch){
	cerr << "instruction e200z4_fnmsub_fra_frb_frc_frd_noStatus have no behavior defined" << endl;
}
void e200z4_fnmsub_fra_frb_frc_frd_useStatus::execute(arch *_arch){
	cerr << "instruction e200z4_fnmsub_fra_frb_frc_frd_useStatus have no behavior defined" << endl;
}
void e200z4_fnmsubs_fra_frb_frc_frd_noStatus::execute(arch *_arch){
	cerr << "instruction e200z4_fnmsubs_fra_frb_frc_frd_noStatus have no behavior defined" << endl;
}
void e200z4_fnmsubs_fra_frb_frc_frd_useStatus::execute(arch *_arch){
	cerr << "instruction e200z4_fnmsubs_fra_frb_frc_frd_useStatus have no behavior defined" << endl;
}
void e200z4_fra_frb_frc_frd_fsel_noStatus::execute(arch *_arch){
	cerr << "instruction e200z4_fra_frb_frc_frd_fsel_noStatus have no behavior defined" << endl;
}
void e200z4_fra_frb_frc_frd_fsel_useStatus::execute(arch *_arch){
	cerr << "instruction e200z4_fra_frb_frc_frd_fsel_useStatus have no behavior defined" << endl;
}
void e200z4_fra_frb_frd_fsub_noStatus::execute(arch *_arch){
	cerr << "instruction e200z4_fra_frb_frd_fsub_noStatus have no behavior defined" << endl;
}
void e200z4_fra_frb_frd_fsub_useStatus::execute(arch *_arch){
	cerr << "instruction e200z4_fra_frb_frd_fsub_useStatus have no behavior defined" << endl;
}
void e200z4_fra_frb_frd_fsubs_noStatus::execute(arch *_arch){
	cerr << "instruction e200z4_fra_frb_frd_fsubs_noStatus have no behavior defined" << endl;
}
void e200z4_fra_frb_frd_fsubs_useStatus::execute(arch *_arch){
	cerr << "instruction e200z4_fra_frb_frd_fsubs_useStatus have no behavior defined" << endl;
}
void e200z4_frb_frd_fres_noStatus::execute(arch *_arch){
	cerr << "instruction e200z4_frb_frd_fres_noStatus have no behavior defined" << endl;
}
void e200z4_frb_frd_fres_useStatus::execute(arch *_arch){
	cerr << "instruction e200z4_frb_frd_fres_useStatus have no behavior defined" << endl;
}
void e200z4_frb_frd_frsp_noStatus::execute(arch *_arch){
	cerr << "instruction e200z4_frb_frd_frsp_noStatus have no behavior defined" << endl;
}
void e200z4_frb_frd_frsp_useStatus::execute(arch *_arch){
	cerr << "instruction e200z4_frb_frd_frsp_useStatus have no behavior defined" << endl;
}
void e200z4_frb_frd_frsqrte_noStatus::execute(arch *_arch){
	cerr << "instruction e200z4_frb_frd_frsqrte_noStatus have no behavior defined" << endl;
}
void e200z4_frb_frd_frsqrte_useStatus::execute(arch *_arch){
	cerr << "instruction e200z4_frb_frd_frsqrte_useStatus have no behavior defined" << endl;
}
void e200z4_frb_frd_fsqrt_noStatus::execute(arch *_arch){
	cerr << "instruction e200z4_frb_frd_fsqrt_noStatus have no behavior defined" << endl;
}
void e200z4_frb_frd_fsqrt_useStatus::execute(arch *_arch){
	cerr << "instruction e200z4_frb_frd_fsqrt_useStatus have no behavior defined" << endl;
}
void e200z4_frb_frd_fsqrts_noStatus::execute(arch *_arch){
	cerr << "instruction e200z4_frb_frd_fsqrts_noStatus have no behavior defined" << endl;
}
void e200z4_frb_frd_fsqrts_useStatus::execute(arch *_arch){
	cerr << "instruction e200z4_frb_frd_fsqrts_useStatus have no behavior defined" << endl;
}
void e200z4_frd_fs_load_notu_ra_rb_x::execute(arch *_arch){
	cerr << "instruction e200z4_frd_fs_load_notu_ra_rb_x have no behavior defined" << endl;
}
void e200z4_frd_fs_load_ra_rb_u_x::execute(arch *_arch){
	cerr << "instruction e200z4_frd_fs_load_ra_rb_u_x have no behavior defined" << endl;
}
void e200z4_frd_mffs_noStatus::execute(arch *_arch){
	cerr << "instruction e200z4_frd_mffs_noStatus have no behavior defined" << endl;
}
void e200z4_frd_mffs_useStatus::execute(arch *_arch){
	cerr << "instruction e200z4_frd_mffs_useStatus have no behavior defined" << endl;
}
void e200z4_frs_fs_notu_ra_rb_store_x::execute(arch *_arch){
	cerr << "instruction e200z4_frs_fs_notu_ra_rb_store_x have no behavior defined" << endl;
}
void e200z4_frs_fs_ra_rb_store_u_x::execute(arch *_arch){
	cerr << "instruction e200z4_frs_fs_ra_rb_store_u_x have no behavior defined" << endl;
}
void e200z4_i_load_nb_notu_ra_rd_sw::execute(arch *_arch){
	cerr << "instruction e200z4_i_load_nb_notu_ra_rd_sw have no behavior defined" << endl;
}
void e200z4_i_nb_notu_ra_rs_store_sw::execute(arch *_arch){
	cerr << "instruction e200z4_i_nb_notu_ra_rs_store_sw have no behavior defined" << endl;
}
void e200z4_icbi_ra_rb::execute(arch *_arch){
	cerr << "instruction e200z4_icbi_ra_rb have no behavior defined" << endl;
}
void e200z4_isync::execute(arch *_arch){
	cerr << "instruction e200z4_isync have no behavior defined" << endl;
}
void e200z4_load_notu_ra_rb_rd_sw_x::execute(arch *_arch){
	cerr << "instruction e200z4_load_notu_ra_rb_rd_sw_x have no behavior defined" << endl;
}
void e200z4_mfsr_rd_sr::execute(arch *_arch){
	cerr << "instruction e200z4_mfsr_rd_sr have no behavior defined" << endl;
}
void e200z4_mfsrin_rb_rd::execute(arch *_arch){
	cerr << "instruction e200z4_mfsrin_rb_rd have no behavior defined" << endl;
}
void e200z4_mftb_rd_tbr::execute(arch *_arch){
	cerr << "instruction e200z4_mftb_rd_tbr have no behavior defined" << endl;
}
void e200z4_mtsr_rs_sr::execute(arch *_arch){
	cerr << "instruction e200z4_mtsr_rs_sr have no behavior defined" << endl;
}
void e200z4_mtsrin_rb_rs::execute(arch *_arch){
	cerr << "instruction e200z4_mtsrin_rb_rs have no behavior defined" << endl;
}
void e200z4_notu_ra_rb_rs_store_sw_x::execute(arch *_arch){
	cerr << "instruction e200z4_notu_ra_rb_rs_store_sw_x have no behavior defined" << endl;
}
void e200z4_rb_tlbie::execute(arch *_arch){
	cerr << "instruction e200z4_rb_tlbie have no behavior defined" << endl;
}
void e200z4_sync::execute(arch *_arch){
	cerr << "instruction e200z4_sync have no behavior defined" << endl;
}
void e200z4_tlbia::execute(arch *_arch){
	cerr << "instruction e200z4_tlbia have no behavior defined" << endl;
}
void e200z4_tlbsync::execute(arch *_arch){
	cerr << "instruction e200z4_tlbsync have no behavior defined" << endl;
}
