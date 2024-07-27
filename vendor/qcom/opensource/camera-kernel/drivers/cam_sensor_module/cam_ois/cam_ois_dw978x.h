#ifndef _CAM_OIS_DW978X_H_
#define _CAM_OIS_DW978X_H_

#include <linux/cma.h>
#include "cam_ois_dev.h"

#define OIS_NAME_LEN 32

int cam_ois_update_time_dw9781c(struct i2c_settings_array *i2c_set);

void swap_low_high_byte(uint32_t *data);

int cam_ois_dw9781c_fw_download_init(struct cam_ois_ctrl_t *o_ctrl);

int cam_ois_dw9784_fw_download_init(struct cam_ois_ctrl_t *o_ctrl);

int cam_ois_dw978x_reset(struct cam_ois_ctrl_t *o_ctrl);

bool cam_ois_dw9781c_need_update_fw(struct cam_ois_ctrl_t *o_ctrl);

bool cam_ois_dw9784_need_update_fw(struct cam_ois_ctrl_t *o_ctrl);

bool cam_ois_dw9784_hi847_need_update_fw(struct cam_ois_ctrl_t *o_ctrl);

int cam_ois_dw9781c_fw_download(struct cam_ois_ctrl_t *o_ctrl);

int cam_ois_dw9784_fw_download(struct cam_ois_ctrl_t *o_ctrl);

uint32_t cam_ois_read_firmware_ver(struct cam_ois_ctrl_t *o_ctrl);

uint32_t cam_ois_dw978x_firmware_download(struct cam_ois_ctrl_t *o_ctrl);

uint32_t cam_ois_dw9781c_firmware_download(struct cam_ois_ctrl_t *o_ctrl);

uint32_t cam_ois_dw9784_firmware_download(struct cam_ois_ctrl_t *o_ctrl);

#endif
/* _CAM_OIS_DW978X_H_ */
