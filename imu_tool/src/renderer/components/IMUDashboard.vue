<template>
  <v-container fluid grid-list-md text-xs-center style="min-width: 1000px;">
    <v-layout row align-center justify-center>
      <v-flex xs4>
        <div :width="400" :height="550">
          <compass :value="value" :width="350" :height="350"> </compass>
        </div>
      </v-flex>
      
      <v-flex xs8>
        <orientation-three :width="550" :height="550" :orientation="orientation"></orientation-three>
      </v-flex>
    </v-layout>
  </v-container>
 
</template>

<script>
  import Compass from '@/components/Compass'
  import OrientationThree from '@/components/OrientationThree'
  import {ImuComm} from '@/imu_comm_ws'

  export default {
    name: 'IMUDashboard',
    components: { Compass, OrientationThree },
    methods: {
      onImuOrientation (o) {
        // console.log('compass orient: ' + o.data.yaw)
        this.value = o.data.yaw

        if (this.value < 0 || this.value > 360) {
          console.log('******* holy shit...... ' + this.value)
        }

        this.orientation = {
          roll: o.data.roll,
          pitch: o.data.pitch,
          yaw: o.data.yaw
        }
      }
    },
    created () {
      ImuComm.putOrientationMode()

      ImuComm.$on('imuOrientation', this.onImuOrientation)
    },
    beforeDestroy () {
      ImuComm.$off('imuOrientation', this.onImuOrientation)
    },
    data () {
      return {
        value: 0,
        width: 500,
        height: 500,
        orientation: {
          roll: 0,
          pitch: 0,
          yaw: 0
        }
      }
    },
    watch: {
    }
  }
</script>

<style scoped>
</style>
