/*********************************************************************
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2014-2021, Dataspeed Inc.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of Dataspeed Inc. nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *********************************************************************/

#include <lusb/UsbDevice.hpp>
#include <stdio.h> // printf()
#include <unistd.h> // sleep()

static void callback(const void *data, int size) {
  (void)data;
  printf("Received %i bytes\n", size);
}

int main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;

  {
    printf("Listing all USB devices:\n");
    std::vector<lusb::UsbDevice::Location> list;
    lusb::UsbDevice::listDevices(0, 0, list);
    for (size_t i = 0; i < list.size(); i++) {
      printf("Bus: %03u, Addr: %03u, Port: %03u, VID: %04x, PID: %04x\n",
          list[i].bus,
          list[i].addr,
          list[i].port,
          list[i].vid,
          list[i].pid
      );
    }
    printf("\n");
  }
  {
    printf("Listing specific USB devices:\n");
    const std::vector<lusb::UsbDevice::UsbIds> ids {
      lusb::UsbDevice::UsbIds(0x0123, 0x4567),
      lusb::UsbDevice::UsbIds(0x89ab, 0xcdef),
    };
    std::vector<lusb::UsbDevice::Location> list;
    lusb::UsbDevice::listDevices(ids, list);
    for (size_t i = 0; i < list.size(); i++) {
      printf("Bus: %03u, Addr: %03u, aaa: %03u, VID: %04x, PID: %04x\n",
          list[i].bus,
          list[i].addr,
          list[i].port,
          list[i].vid,
          list[i].pid
      );
    }
    printf("\n");
  }
  {
    printf("Opening 1st device with VID 1234\n");
    lusb::UsbDevice dev(0x1234, 0);
    if (dev.open()) {
      printf("Writing 64 bytes to endpoint 1 with 10ms timeout\n");
      std::vector<uint8_t> data(64);
      if (dev.bulkWrite(data.data(), data.size(), 1, 10)) {
        printf("Reading 64 bytes from endpoint 1 with 10ms timeout\n");
        std::vector<uint8_t> data(64);
        if (dev.bulkRead(data.data(), data.size(), 1, 10)) {
          printf("Starting bulk read thread on endpoint 1\n");
          dev.startBulkReadThread(&callback, 1);
          usleep(2);
          dev.stopBulkReadThread(1);
        } else {
          printf("Read failed\n");
        }
      } else {
        printf("Write failed\n");
      }
      dev.close();
    } else {
      printf("Failed to open device\n");
    }
  }

  return 0;
}
