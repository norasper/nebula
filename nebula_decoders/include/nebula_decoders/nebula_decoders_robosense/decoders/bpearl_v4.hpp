#pragma once

#include "nebula_decoders/nebula_decoders_robosense/decoders/robosense_packet.hpp"
#include "nebula_decoders/nebula_decoders_robosense/decoders/robosense_sensor.hpp"

#include "boost/endian/buffers.hpp"

#include <cstddef>
#include <cstdint>

namespace nebula
{
namespace drivers
{
namespace robosense_packet
{
namespace bpearl_v4
{
#pragma pack(push, 1)

struct Header
{
  boost::endian::big_uint64_buf_t header_id;
  boost::endian::big_uint32_buf_t reserved_first;
  boost::endian::big_uint32_buf_t packet_count;
  boost::endian::big_uint32_buf_t reserved_second;
  Timestamp timestamp;
  boost::endian::big_uint8_buf_t lidar_type;
  boost::endian::big_uint8_buf_t lidar_model;
  uint8_t reserved_third[9];
};

struct Packet : public PacketBase<12, 32, 2, 100>
{
  typedef Body<Block<Unit, Packet::N_CHANNELS>, Packet::N_BLOCKS> body_t;
  Header header;
  body_t body;
  boost::endian::big_uint48_buf_t tail;
};

struct OperatingStatus
{
  boost::endian::big_uint8_buf_t reserved_first;
  boost::endian::big_uint16_buf_t machine_current;
  boost::endian::big_uint24_buf_t reserved_second;
  boost::endian::big_uint16_buf_t machine_voltage;
};

struct FaultDiagnosis
{
  boost::endian::big_uint16_buf_t startup_times;
  boost::endian::big_uint32_buf_t reserved;
  boost::endian::big_uint8_buf_t gps_status;
  boost::endian::big_uint16_buf_t machine_temp;
  uint8_t reserved_first[11];
  boost::endian::big_uint16_buf_t phase;
  boost::endian::big_uint16_buf_t rotation_speed;
};

struct InfoPacket
{
  boost::endian::big_uint64_buf_t header;
  boost::endian::big_uint16_buf_t motor_speed_setting;
  Ethernet ethernet;
  FovSetting fov_setting;
  boost::endian::big_uint16_buf_t tcp_msop_port;
  boost::endian::big_uint16_buf_t phase_lock;
  FirmwareVersion mainboard_firmware_version;
  FirmwareVersion bottom_firmware_version;
  FirmwareVersion app_software_version;
  FirmwareVersion motor_firmware_version;
  uint8_t reserved_first[228];
  boost::endian::big_uint8_buf_t baud_rate;
  uint8_t reserved_second[3];
  SerialNumber serial_number;
  uint8_t reserved_third[2];
  boost::endian::big_uint8_buf_t return_mode;
  boost::endian::big_uint8_buf_t time_sync_mode;
  boost::endian::big_uint8_buf_t sync_status;
  OperatingStatus operating_status;
  boost::endian::big_uint8_buf_t rotation_direction;
  boost::endian::big_uint32_buf_t running_time;
  uint8_t reserved_fourth[9];
  FaultDiagnosis fault_diagnosis;
  uint8_t reserved_fifth[7];
  boost::endian::big_uint8_buf_t gprmc[86];
  SensorCalibration sensor_calibration;
  uint8_t reserved_sixth[586];
  boost::endian::big_uint16_buf_t tail;
};

#pragma pack(pop)
}  // namespace bpearl_v4

/// @brief Get the distance unit of the given @ref BpearlV3 packet in meters.
/// @return 0.0025m (0.25cm)
template <>
double get_dis_unit<bpearl_v4::Packet>(const bpearl_v4::Packet & /* packet */)
{
  return 0.0025;
}

}  // namespace robosense_packet

class BpearlV4 : public RobosenseSensor<
                   robosense_packet::bpearl_v4::Packet, robosense_packet::bpearl_v4::InfoPacket>
{
private:
  static constexpr int firing_time_offset_ns_single_[12][32] = {
    {0,    167,  334,  500,  667,  834,  1001, 1168, 1334, 1501, 1668,
     1835, 2002, 2168, 2335, 2502, 2669, 2836, 3002, 3169, 3336, 3503,
     3670, 3836, 4003, 4170, 4337, 4504, 4670, 4837, 5004, 5171},
    {5555, 5722, 5889, 6055, 6222, 6389,  6556,  6723,  6889,  7056, 7223,
     7390, 7557, 7723, 7890, 8057, 8224,  8391,  8557,  8724,  8891, 9058,
     9225, 9391, 9558, 9725, 9892, 10059, 10225, 10392, 10559, 10726},
    {11110, 11277, 11444, 11610, 11777, 11944, 12111, 12278, 12444, 12611, 12778,
     12945, 13112, 13278, 13445, 13612, 13779, 13946, 14112, 14279, 14446, 14613,
     14780, 14946, 15113, 15280, 15447, 15614, 15780, 15947, 16114, 16281},
    {16665, 16832, 16999, 17165, 17332, 17499, 17666, 17833, 17999, 18166, 18333,
     18500, 18667, 18833, 19000, 19167, 19334, 19501, 19667, 19834, 20001, 20168,
     20335, 20501, 20668, 20835, 21002, 21169, 21335, 21502, 21669, 21836},
    {22220, 22387, 22554, 22720, 22887, 23054, 23221, 23388, 23554, 23721, 23888,
     24055, 24222, 24388, 24555, 24722, 24889, 25056, 25222, 25389, 25556, 25723,
     25890, 26056, 26223, 26390, 26557, 26724, 26890, 27057, 27224, 27391},
    {27775, 27942, 28109, 28275, 28442, 28609, 28776, 28943, 29109, 29276, 29443,
     29610, 29777, 29943, 30110, 30277, 30444, 30611, 30777, 30944, 31111, 31278,
     31445, 31611, 31778, 31945, 32112, 32279, 32445, 32612, 32779, 32946},
    {33330, 33497, 33664, 33830, 33997, 34164, 34331, 34498, 34664, 34831, 34998,
     35165, 35332, 35498, 35665, 35832, 35999, 36166, 36332, 36499, 36666, 36833,
     37000, 37166, 37333, 37500, 37667, 37834, 38000, 38167, 38334, 38501},
    {38885, 39052, 39219, 39385, 39552, 39719, 39886, 40053, 40219, 40386, 40553,
     40720, 40887, 41053, 41220, 41387, 41554, 41721, 41887, 42054, 42221, 42388,
     42555, 42721, 42888, 43055, 43222, 43389, 43555, 43722, 43889, 44056},
    {44440, 44607, 44774, 44940, 45107, 45274, 45441, 45608, 45774, 45941, 46108,
     46275, 46442, 46608, 46775, 46942, 47109, 47276, 47442, 47609, 47776, 47943,
     48110, 48276, 48443, 48610, 48777, 48944, 49110, 49277, 49444, 49611},
    {49995, 50162, 50329, 50495, 50662, 50829, 50996, 51163, 51329, 51496, 51663,
     51830, 51997, 52163, 52330, 52497, 52664, 52831, 52997, 53164, 53331, 53498,
     53665, 53831, 53998, 54165, 54332, 54499, 54665, 54832, 54999, 55166},
    {55550, 55717, 55884, 56050, 56217, 56384, 56551, 56718, 56884, 57051, 57218,
     57385, 57552, 57718, 57885, 58052, 58219, 58386, 58552, 58719, 58886, 59053,
     59220, 59386, 59553, 59720, 59887, 60054, 60220, 60387, 60554, 60721},
    {61105, 61272, 61439, 61605, 61772, 61939, 62106, 62273, 62439, 62606, 62773,
     62940, 63107, 63273, 63440, 63607, 63774, 63941, 64107, 64274, 64441, 64608,
     64775, 64941, 65108, 65275, 65442, 65609, 65775, 65942, 66109, 66276}};

  static constexpr int firing_time_offset_ns_dual_[12][32]{
    {0,    167,  334,  500,  667,  834,  1001, 1168, 1334, 1501, 1668,
     1835, 2002, 2168, 2335, 2502, 2669, 2836, 3002, 3169, 3336, 3503,
     3670, 3836, 4003, 4170, 4337, 4504, 4670, 4837, 5004, 5171},
    {0,    167,  334,  500,  667,  834,  1001, 1168, 1334, 1501, 1668,
     1835, 2002, 2168, 2335, 2502, 2669, 2836, 3002, 3169, 3336, 3503,
     3670, 3836, 4003, 4170, 4337, 4504, 4670, 4837, 5004, 5171},
    {5555, 5722, 5889, 6055, 6222, 6389,  6556,  6723,  6889,  7056, 7223,
     7390, 7557, 7723, 7890, 8057, 8224,  8391,  8557,  8724,  8891, 9058,
     9225, 9391, 9558, 9725, 9892, 10059, 10225, 10392, 10559, 10726},
    {5555, 5722, 5889, 6055, 6222, 6389,  6556,  6723,  6889,  7056, 7223,
     7390, 7557, 7723, 7890, 8057, 8224,  8391,  8557,  8724,  8891, 9058,
     9225, 9391, 9558, 9725, 9892, 10059, 10225, 10392, 10559, 10726},
    {11110, 11277, 11444, 11610, 11777, 11944, 12111, 12278, 12444, 12611, 12778,
     12945, 13112, 13278, 13445, 13612, 13779, 13946, 14112, 14279, 14446, 14613,
     14780, 14946, 15113, 15280, 15447, 15614, 15780, 15947, 16114, 16281},
    {11110, 11277, 11444, 11610, 11777, 11944, 12111, 12278, 12444, 12611, 12778,
     12945, 13112, 13278, 13445, 13612, 13779, 13946, 14112, 14279, 14446, 14613,
     14780, 14946, 15113, 15280, 15447, 15614, 15780, 15947, 16114, 16281},
    {16665, 16832, 16999, 17165, 17332, 17499, 17666, 17833, 17999, 18166, 18333,
     18500, 18667, 18833, 19000, 19167, 19334, 19501, 19667, 19834, 20001, 20168,
     20335, 20501, 20668, 20835, 21002, 21169, 21335, 21502, 21669, 21836},
    {16665, 16832, 16999, 17165, 17332, 17499, 17666, 17833, 17999, 18166, 18333,
     18500, 18667, 18833, 19000, 19167, 19334, 19501, 19667, 19834, 20001, 20168,
     20335, 20501, 20668, 20835, 21002, 21169, 21335, 21502, 21669, 21836},
    {22220, 22387, 22554, 22720, 22887, 23054, 23221, 23388, 23554, 23721, 23888,
     24055, 24222, 24388, 24555, 24722, 24889, 25056, 25222, 25389, 25556, 25723,
     25890, 26056, 26223, 26390, 26557, 26724, 26890, 27057, 27224, 27391},
    {22220, 22387, 22554, 22720, 22887, 23054, 23221, 23388, 23554, 23721, 23888,
     24055, 24222, 24388, 24555, 24722, 24889, 25056, 25222, 25389, 25556, 25723,
     25890, 26056, 26223, 26390, 26557, 26724, 26890, 27057, 27224, 27391},
    {27775, 27942, 28109, 28275, 28442, 28609, 28776, 28943, 29109, 29276, 29443,
     29610, 29777, 29943, 30110, 30277, 30444, 30611, 30777, 30944, 31111, 31278,
     31445, 31611, 31778, 31945, 32112, 32279, 32445, 32612, 32779, 32946},
    {27775, 27942, 28109, 28275, 28442, 28609, 28776, 28943, 29109, 29276, 29443,
     29610, 29777, 29943, 30110, 30277, 30444, 30611, 30777, 30944, 31111, 31278,
     31445, 31611, 31778, 31945, 32112, 32279, 32445, 32612, 32779, 32946}};

public:
  static constexpr float MIN_RANGE = 0.1f;
  static constexpr float MAX_RANGE = 30.f;
  static constexpr size_t MAX_SCAN_BUFFER_POINTS = 1152000;

  int getPacketRelativePointTimeOffset(
    uint32_t block_id, uint32_t channel_id,
    const std::shared_ptr<RobosenseSensorConfiguration> & sensor_configuration) override
  {
    if (sensor_configuration->return_mode == ReturnMode::DUAL)
      return firing_time_offset_ns_dual_[block_id][channel_id];
    else
      return firing_time_offset_ns_single_[block_id][channel_id];
  }

  ReturnMode getReturnMode(const robosense_packet::bpearl_v4::InfoPacket & info_packet)
  {
    const uint8_t return_mode_data = info_packet.return_mode.value();
    if (return_mode_data == 0x00) {
      return ReturnMode::DUAL;
    } else if (return_mode_data == 0x04) {
      return ReturnMode::SINGLE_STRONGEST;
    } else if (return_mode_data == 0x05) {
      return ReturnMode::SINGLE_LAST;
    } else if (return_mode_data == 0x06) {
      return ReturnMode::SINGLE_FIRST;
    }
    return ReturnMode::UNKNOWN;
  }

  RobosenseCalibrationConfiguration getSensorCalibration(
    const robosense_packet::bpearl_v4::InfoPacket & info_packet)
  {
    return info_packet.sensor_calibration.getCalibration();
  }

  std::map<std::string, std::string> getSensorInfo(
    const robosense_packet::bpearl_v4::InfoPacket & info_packet)
  {
    std::map<std::string, std::string> sensor_info;
    sensor_info["motor_speed"] = std::to_string(info_packet.motor_speed_setting.value());
    sensor_info["lidar_ip"] = info_packet.ethernet.lidar_ip.to_string();
    sensor_info["dest_pc_ip"] = info_packet.ethernet.dest_pc_ip.to_string();
    sensor_info["mac_addr"] = info_packet.ethernet.mac_addr.to_string();
    sensor_info["lidar_out_msop_port"] =
      std::to_string(info_packet.ethernet.lidar_out_msop_port.value());
    sensor_info["lidar_out_difop_port"] =
      std::to_string(info_packet.ethernet.lidar_out_difop_port.value());
    sensor_info["fov_start"] = std::to_string(info_packet.fov_setting.fov_start.value());
    sensor_info["fov_end"] = std::to_string(info_packet.fov_setting.fov_end.value());
    sensor_info["tcp_msop_port"] = std::to_string(info_packet.tcp_msop_port.value());
    sensor_info["phase_lock"] = std::to_string(info_packet.phase_lock.value());
    sensor_info["mainboard_firmware_version"] = info_packet.mainboard_firmware_version.to_string();
    sensor_info["bottom_firmware_version"] = info_packet.bottom_firmware_version.to_string();
    sensor_info["app_software_version"] = info_packet.app_software_version.to_string();
    sensor_info["motor_firmware_version"] = info_packet.motor_firmware_version.to_string();
    sensor_info["baud_rate"] = std::to_string(info_packet.baud_rate.value());
    sensor_info["serial_number"] = info_packet.serial_number.to_string();

    if (info_packet.return_mode.value() == 0x00) {
      sensor_info["return_mode"] = "dual";
    } else if (info_packet.return_mode.value() == 0x04) {
      sensor_info["return_mode"] = "strongest";
    } else if (info_packet.return_mode.value() == 0x05) {
      sensor_info["return_mode"] = "last";
    } else if (info_packet.return_mode.value() == 0x06) {
      sensor_info["return_mode"] = "first";
    }
    sensor_info["time_sync_mode"] = std::to_string(info_packet.time_sync_mode.value());
    sensor_info["sync_status"] = std::to_string(info_packet.sync_status.value());
    sensor_info["machine_current"] =
      std::to_string(info_packet.operating_status.machine_current.value());
    sensor_info["machine_voltage"] =
      std::to_string(info_packet.operating_status.machine_voltage.value());
    sensor_info["rotation_direction"] = std::to_string(info_packet.rotation_direction.value());
    sensor_info["running_time"] = std::to_string(info_packet.running_time.value());
    sensor_info["startup_times"] =
      std::to_string(info_packet.fault_diagnosis.startup_times.value());

    const uint8_t gps_status_data = info_packet.fault_diagnosis.gps_status.value();
    if ((gps_status_data & 0b10000000) == 0b10000000) {
      sensor_info["pps_lock"] = "valid";
    } else {
      sensor_info["pps_lock"] = "invalid";
    }
    if ((gps_status_data & 0b01000000) == 0b01000000) {
      sensor_info["gprmc_lock"] = "valid";
    } else {
      sensor_info["gprmc_lock"] = "invalid";
    }
    if ((gps_status_data & 0b00100000) == 0b00100000) {
      sensor_info["utc_lock"] = "synchronized";
    } else {
      sensor_info["utc_lock"] = "not_synchronized";
    }
    if ((gps_status_data & 0b00010000) == 0b00010000) {
      sensor_info["pps_input_status"] = "input_present";
    } else {
      sensor_info["pps_input_status"] = "no_input";
    }

    sensor_info["machine_temp"] = std::to_string(info_packet.fault_diagnosis.machine_temp.value());
    sensor_info["phase"] = std::to_string(info_packet.fault_diagnosis.phase.value());
    sensor_info["rotation_speed"] =
      std::to_string(info_packet.fault_diagnosis.rotation_speed.value());

    //        sensor_info["gprmc"] = std::to_string(info_packet.gprmc.getGprmc());

    return sensor_info;
  }
};
}  // namespace drivers
}  // namespace nebula