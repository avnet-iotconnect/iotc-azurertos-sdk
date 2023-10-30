#ifndef __HS300X_SENSOR_THREAD_ENTRY_H__
#define __HS300X_SENSOR_THREAD_ENTRY_H__

// in hs300x_sensor_thread_entry.c
void hs300x_sensor_thread_entry(unsigned long entry_input);
double get_temperature();
double get_humidity();

#endif /* __HS300X_SENSOR_THREAD_ENTRY_H__ */
