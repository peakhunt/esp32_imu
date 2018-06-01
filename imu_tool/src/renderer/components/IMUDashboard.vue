<template>
  <v-container grid-list-md text-xs-center>
    <v-layout row wrap>
      <v-flex xs6>
        <compass :value="value" :width="width" :height="height"> </compass>
      </v-flex>
      
      <v-flex xs6>
        <orientation-three :width="width" :height="height"></orientation-three>
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
        width: 400,
        height: 400
      }
    },
    watch: {
    }
  }
</script>

<style scoped>
</style>
