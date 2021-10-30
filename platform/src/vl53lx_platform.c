#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <linux/i2c-dev.h>
#include <unistd.h>
#include <sys/uio.h>
#include "vl53lx_platform.h"
#include "vl53lx_api.h"

int VL53LX_i2c_init(char * devPath, int devAddr)
{
    int file;
    if ((file = open(devPath, O_RDWR)) < 0) {
        /* ERROR HANDLING: you can check errno to see what went wrong */
        perror("Failed to open the i2c bus");
        return -1;
    }
    if (ioctl(file, I2C_SLAVE, devAddr) < 0) {
        printf("Failed to acquire bus access and/or talk to slave.\n");
        /* ERROR HANDLING; you can check errno to see what went wrong */
        return -1;
    }
    if (ioctl(file, I2C_TENBIT, 1) < 0) {
      printf("Failed to set ten bit addressing.\n");
      /* ERROR HANDLING; you can check errno to see what went wrong */
      return -1;
    }
    return file;
}

int32_t VL53LX_i2c_close(void)
{
    printf("%s\n", __FUNCTION__);
    return VL53LX_ERROR_NOT_IMPLEMENTED;
}

static int i2c_write(int fd, uint16_t cmd, uint8_t * data, uint8_t len){

#if 0
    // Looks like i2c-dev doesn't support writev
    struct iovec vec[2];
    uint8_t buf[2] = { cmd >> 8, cmd & 0xff };
    vec[0].iov_base = buf;
    vec[0].iov_len = 2;
    vec[1].iov_base = data;
    vec[2].iov_len = len;
    if (writev(fd, vec, 2) != len+2) {
        printf("Failed to write to the i2c bus due to %s.\n", strerror(errno));
        return VL53LX_ERROR_CONTROL_INTERFACE;
    }
#else
    uint8_t *buf = malloc(len + 2);
    buf[0] = cmd >> 8;
    buf[1] = cmd & 0xff;
    memcpy(buf + 2, data, len);
    if (write(fd, buf, len + 2) != len+2) {
        free(buf);
        printf("Failed to write to the i2c bus due to %s.\n", strerror(errno));
        return VL53LX_ERROR_CONTROL_INTERFACE;
    }
    free(buf);
#endif
    return VL53LX_ERROR_NONE;
}

static int i2c_read(int fd, uint16_t cmd, uint8_t * data, uint8_t len){

    uint8_t buf[2] = { cmd >> 8, cmd & 0xff };
    if (write(fd, buf, 2) != 2) {
        printf("Failed to write to the i2c bus due to %s.\n", strerror(errno));
        return VL53LX_ERROR_CONTROL_INTERFACE;
    }

    if (read(fd, data, len) != len) {
        printf("Failed to read from the i2c bus due to %s.\n", strerror(errno));
        return VL53LX_ERROR_CONTROL_INTERFACE;
    }

    return VL53LX_ERROR_NONE;
}

VL53LX_Error VL53LX_LockSequenceAccess(VL53LX_DEV Dev){
    VL53LX_Error Status = VL53LX_ERROR_NONE;
    return Status;
}

VL53LX_Error VL53LX_UnlockSequenceAccess(VL53LX_DEV Dev){
    VL53LX_Error Status = VL53LX_ERROR_NONE;
    return Status;
}

VL53LX_Error VL53LX_WriteMulti(VL53LX_DEV Dev, uint16_t index, uint8_t *pdata, uint32_t count){
    return i2c_write(Dev->fd, index, pdata, count);
}

VL53LX_Error VL53LX_ReadMulti(VL53LX_DEV Dev, uint16_t index, uint8_t *pdata, uint32_t count){
    return i2c_read(Dev->fd, index, pdata, count);
}

VL53LX_Error VL53LX_WrByte(VL53LX_DEV Dev, uint16_t index, uint8_t data){
	return i2c_write(Dev->fd, index, &data, 1);
}

VL53LX_Error VL53LX_WrWord(VL53LX_DEV Dev, uint16_t index, uint16_t data){
    uint8_t buf[4];
    buf[1] = data>>0&0xFF;
    buf[0] = data>>8&0xFF;
    return i2c_write(Dev->fd, index, buf, 2);
}

VL53LX_Error VL53LX_WrDWord(VL53LX_DEV Dev, uint16_t index, uint32_t data){
    uint8_t buf[4];
    buf[3] = data>>0&0xFF;
    buf[2] = data>>8&0xFF;
    buf[1] = data>>16&0xFF;
    buf[0] = data>>24&0xFF;
    return i2c_write(Dev->fd, index, buf, 4);
}

VL53LX_Error VL53LX_UpdateByte(VL53LX_DEV Dev, uint16_t index, uint8_t AndData, uint8_t OrData){

    int32_t status_int;
    uint8_t data;

    status_int = i2c_read(Dev->fd, index, &data, 1);

    if (status_int != 0){
        return  status_int;
    }

    data = (data & AndData) | OrData;
    return i2c_write(Dev->fd, index, &data, 1);
}

VL53LX_Error VL53LX_RdByte(VL53LX_DEV Dev, uint16_t index, uint8_t *data){
    uint8_t tmp = 0;
    int ret = i2c_read(Dev->fd, index, &tmp, 1);
    *data = tmp;
    // printf("%u\n", tmp);
    return ret;
}

VL53LX_Error VL53LX_RdWord(VL53LX_DEV Dev, uint16_t index, uint16_t *data){
    uint8_t buf[2];
    int ret = i2c_read(Dev->fd, index, buf, 2);
    uint16_t tmp = 0;
    tmp |= buf[1]<<0;
    tmp |= buf[0]<<8;
    // printf("%u\n", tmp);
    *data = tmp;
    return ret;
}

VL53LX_Error  VL53LX_RdDWord(VL53LX_DEV Dev, uint16_t index, uint32_t *data){
    uint8_t buf[4];
    int ret = i2c_read(Dev->fd, index, buf, 4);
    uint32_t tmp = 0;
    tmp |= buf[3]<<0;
    tmp |= buf[2]<<8;
    tmp |= buf[1]<<16;
    tmp |= buf[0]<<24;
    *data = tmp;
    // printf("%zu\n", tmp);
    return ret;
}

VL53LX_Error VL53LX_PollingDelay(VL53LX_DEV Dev){
    usleep(5000);
    return VL53LX_ERROR_NONE;
}

VL53LX_Error VL53LX_WaitUs(
		VL53LX_Dev_t *pdev,
		int32_t       wait_us){
    usleep(wait_us);
    return VL53LX_ERROR_NONE;
}

VL53LX_Error VL53LX_WaitMs(
		VL53LX_Dev_t *pdev,
		int32_t       wait_ms){
    usleep(wait_ms*1000);
    return VL53LX_ERROR_NONE;
}

VL53LX_Error VL53LX_GetTickCount(
  VL53LX_Dev_t* pdev,
  uint32_t* ptime_ms) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  *ptime_ms = (uint32_t)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000LL);
  return VL53LX_ERROR_NONE;
}

VL53LX_Error VL53LX_WaitValueMaskEx(
  VL53LX_Dev_t* pdev,
  uint32_t      timeout_ms,
  uint16_t      index,
  uint8_t       value,
  uint8_t       mask,
  uint32_t      poll_delay_ms)
{


  VL53LX_Error status = VL53LX_ERROR_NONE;
  uint32_t     start_time_ms = 0;
  uint32_t     current_time_ms = 0;
  uint32_t     polling_time_ms = 0;
  uint8_t      byte_value = 0;
  uint8_t      found = 0;
#ifdef VL53LX_LOG_ENABLE
  uint32_t     trace_functions = 0;
#endif

  //_LOG_STRING_BUFFER(register_name);

  SUPPRESS_UNUSED_WARNING(poll_delay_ms);

#ifdef VL53LX_LOG_ENABLE

  VL53LX_get_register_name(
    index,
    register_name);


  trace_i2c("WaitValueMaskEx(%5d, %s, 0x%02X, 0x%02X, %5d);\n",
    timeout_ms, register_name, value, mask, poll_delay_ms);
#endif



  VL53LX_GetTickCount(pdev, &start_time_ms);
  //pdev->new_data_ready_poll_duration_ms = 0;



#ifdef VL53LX_LOG_ENABLE
  trace_functions = _LOG_GET_TRACE_FUNCTIONS();
#endif
  //_LOG_SET_TRACE_FUNCTIONS(VL53LX_TRACE_FUNCTION_NONE);



  while ((status == VL53LX_ERROR_NONE) &&
    (polling_time_ms < timeout_ms) &&
    (found == 0))
  {
    status = VL53LX_RdByte(
      pdev,
      index,
      &byte_value);

    if ((byte_value & mask) == value)
    {
      found = 1;
    }




    VL53LX_GetTickCount(pdev, &current_time_ms);
    polling_time_ms = current_time_ms - start_time_ms;
  }


  //_LOG_SET_TRACE_FUNCTIONS(trace_functions);

  if (found == 0 && status == VL53LX_ERROR_NONE)
    status = VL53LX_ERROR_TIME_OUT;

  return status;
}
