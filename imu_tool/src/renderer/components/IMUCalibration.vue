<template>
  <v-container fluid grid-list-md fill mt-0>
    <v-tabs
     v-model="activeTab"
     color="cyan"
     dark
     slider-color="yellow"
    >
      <v-tab :key="0">Gyro Calibration</v-tab>
      <v-tab :key="1">Accelerometer Calibration</v-tab> 
      <v-tab :key="2">Magnetometer Calibration</v-tab>

      <v-tab-item :key="0">
        <v-card flat>
          <v-card-text>Gyro Calibration</v-card-text>
        </v-card>
      </v-tab-item>

      <v-tab-item :key="1">
        <v-card flat>
          <v-card-text>Accelerometer Calibration</v-card-text>
        </v-card>
      </v-tab-item>

      <v-tab-item :key="2">
        <mag-calibration></mag-calibration>
        </v-card>
      </v-tab-item>
    </v-tabs>
  </v-container>
</template>

<script>
  import MagCalibration from '@/components/MAGCalibration'
  import {ImuComm} from '@/imu_comm'

  export default {
    name: 'IMUCalibration',
    components: {
      MagCalibration
    },
    data () {
      return {
        activeTab: 0
      }
    },
    watch: {
      activeTab (ņ, o) {
        switch (this.activeTab) {
          case '0':
            console.log('putting gyro cal mode')
            break

          case '1':
            console.log('putting acceleometer cal mode')
            break

          case '2':
            console.log('putting magnetometer cal mode')
            ImuComm.putMagnetometerMode()
            break
        }
      }
    }
  }
</script>

<style scoped>
</style>
