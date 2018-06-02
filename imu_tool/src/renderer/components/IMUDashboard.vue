<template>
  <v-container grid-list-md text-xs-center>
    <v-layout row wrap align-center justify-center>
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

  export default {
    name: 'IMUDashboard',
    components: { Compass, OrientationThree },
    methods: {
      onImuOrientation (o) {
        // console.log('compass orient: ' + o.data.yaw)
        this.value = o.data.yaw

        this.orientation = {
          roll: o.data.roll,
          pitch: o.data.pitch,
          yaw: o.data.yaw
        }
      }
    },
    created () {
      this.$root.$children[0].$on('imuOrientation', this.onImuOrientation)
    },
    beforeDestroy () {
      this.$root.$children[0].$off('imuOrientation', this.onImuOrientation)
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
