<template>
  <v-card flat>
    <v-container fluid grid-list-md text-xs-center style="min-width: 900px;">
      <v-layout row align-center justify-center>
        <v-flex xs3>
          <v-text-field label="Bias X" v-model.number="bias_mx" disabled></v-text-field>
        </v-flex>
        <v-flex xs3>
          <v-text-field label="Bias Y" v-model.number="bias_my" disabled></v-text-field>
        </v-flex>
        <v-flex xs3>
          <v-text-field label="Bias Z" v-model.numbner="bias_mz" disabled></v-text-field>
        </v-flex>

        <v-flex xs3>
          <v-btn color="primary" dark :disabled="ImuComm.isStopped" @click="startCalibrate()">Calibrate</v-btn>
        </v-flex>

      </v-layout>

      <v-layout row align-center justify-center>
        <v-flex xs6>
          <p>Raw/Unaligned</p>
          <sphere-fitting-view :width="400" :height="400" ref="uncalibated"></sphere-fitting-view>
        </v-flex>
        
        <v-flex xs6>
          <p>Calibration Applied/Sensor Aligned</p>
          <sphere-fitting-view :width="400" :height="400" ref="calibrated"></sphere-fitting-view>
        </v-flex>
      </v-layout>
    </v-container>
    <mag-calibration-dialog :showDialog="showProgDialog" @magCalFinished="calibrationFinished()"></mag-calibration-dialog>
  </v-card>
</template>

<script>
  import SphereFittingView from '@/components/SphereFittingView'
  import MagCalibrationDialog from '@/components/MAGCalibrationDialog'
  import {ImuComm} from '@/imu_comm_ws'

  export default {
    name: 'MagCalibrationVue',
    components: {
      SphereFittingView,
      MagCalibrationDialog
    },
    methods: {
      onImuOrientation (o) {
        if (this.showProgDialog === true) {
          return
        }

        var r, c

        r = o.raw
        c = o.cal

        this.$refs.uncalibated.push_point({
          x: r.mx / 50,
          y: r.my / 50,
          z: r.mz / 50
        })

        this.$refs.calibrated.push_point({
          x: c.mx / 50,
          y: c.my / 50,
          z: c.mz / 50
        })

        this.bias_mx = o.mag_bias.mx
        this.bias_my = o.mag_bias.my
        this.bias_mz = o.mag_bias.mz
      },
      tabSelected () {
        console.log('mag calibration tab selected')

        this.$refs.uncalibated.clear_points()
        this.$refs.calibrated.clear_points()
      },
      startCalibrate () {
        console.log('starting magnetometer calibration')
        this.showProgDialog = true

        this.$refs.uncalibated.clear_points()
        this.$refs.calibrated.clear_points()
      },
      calibrationFinished () {
        console.log('magnetometer calibration finished')
        this.showProgDialog = false
      }
    },
    created () {
      ImuComm.$on('imuMagnetometer', this.onImuOrientation)
    },
    beforeDestroy () {
      ImuComm.$off('imuMagnetometer', this.onImuOrientation)
    },
    data () {
      return {
        bias_mx: 0,
        bias_my: 0,
        bias_mz: 0,
        ImuComm: ImuComm,
        showProgDialog: false
      }
    }
  }
</script>

<style scoped>
</style>
