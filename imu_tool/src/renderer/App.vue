<template>
  <div id="app">
    <v-app dark>
      <v-navigation-drawer
        fixed
        :mini-variant="miniVariant"
        :clipped="clipped"
        v-model="drawer"
        app
      >
        <v-list>
          <v-list-tile 
            router
            :to="item.to"
            :key="i"
            v-for="(item, i) in items"
            exact
          >
            <v-list-tile-action>
              <v-icon v-html="item.icon"></v-icon>
            </v-list-tile-action>
            <v-list-tile-content>
              <v-list-tile-title v-text="item.title"></v-list-tile-title>
            </v-list-tile-content>
          </v-list-tile>
        </v-list>
      </v-navigation-drawer>
      <v-toolbar fixed app :clipped-left="clipped">
        <v-toolbar-side-icon @click.native.stop="drawer = !drawer"></v-toolbar-side-icon>
        <v-btn 
          icon
          @click.native.stop="miniVariant = !miniVariant"
        >
          <v-icon v-html="miniVariant ? 'chevron_right' : 'chevron_left'"></v-icon>
        </v-btn>
        <v-btn
          icon
          @click.native.stop="clipped = !clipped"
        >
          <v-icon>web</v-icon>
        </v-btn>
        <v-btn
          icon
          @click.native.stop="fixed = !fixed"
        >
          <v-icon>remove</v-icon>
        </v-btn>
        <v-toolbar-title v-text="title"></v-toolbar-title>
        <v-spacer></v-spacer>

        <v-badge left>
          <span slot="badge">{{numRequest}}</span>
          <span>Requests</span>
        </v-badge>
        &nbsp;&nbsp;&nbsp;
        &nbsp;&nbsp;&nbsp;

        <v-badge left>
          <span slot="badge">{{numSuccess}}</span>
          <span>Success</span>
        </v-badge>
        &nbsp;&nbsp;&nbsp;
        &nbsp;&nbsp;&nbsp;

        <v-badge left>
          <span slot="badge">{{numFail}}</span>
          <span>Fails</span>
        </v-badge>

        <v-btn v-if="isStopped === true" icon @click="showConnectDialog = true">
          <v-icon>play_arrow</v-icon>
        </v-btn>
        <v-btn v-if="isStopped === false" icon @click="onStopPressed">
          <v-icon>stop</v-icon>
        </v-btn>

        <v-btn v-if="isStopped === true" icon @click="onSimulStart">
          <v-icon>play_arrow</v-icon>
        </v-btn>
        <v-btn v-if="isStopped === false" icon @click="onSimulStop">
          <v-icon>stop</v-icon>
        </v-btn>

      </v-toolbar>

      <v-content>
        <v-container fluid fill-height>
          <v-slide-y-transition mode="out-in">
            <router-view></router-view>
          </v-slide-y-transition>
        </v-container>
      </v-content>

      <v-footer :fixed="fixed" app>
        <v-spacer></v-spacer>
        <span>&copy; 김복돌 2018</span>
      </v-footer>

      <connect-dialog :showDialog="showConnectDialog" @connect="onConnectPressed" @dismiss="onDismissPressed"></connect-dialog>
    </v-app>
  </div>
</template>

<script>
  import ConnectDialog from '@/components/ConnectDialog'

  export default {
    name: 'imu_tool',
    components: { ConnectDialog },
    methods: {
      getRandom (min, max) {
        return Math.random() * (max - min) + min
      },
      onSimulStart () {
        this.isStopped = false

        this.numRequest = 0
        this.numSuccess = 0
        this.numFail = 0

        this.$emit('onConnected')

        this.simulTimer = setInterval(() => {
          var obj = {
            data: {
              roll: this.getRandom(-180, 180),
              pitch: this.getRandom(-180, 180),
              yaw: this.getRandom(0, 360),
              ax: this.getRandom(-2, 2),
              ay: this.getRandom(-2, 2),
              az: this.getRandom(-2, 2),
              gx: this.getRandom(-2, 2),
              gy: this.getRandom(-2, 2),
              gz: this.getRandom(-2, 2),
              mx: this.getRandom(-2, 2),
              my: this.getRandom(-2, 2),
              mz: this.getRandom(-2, 2)
            }
          }
          this.$emit('imuOrientation', obj)
        }, 10)
      },
      onSimulStop () {
        this.$emit('onDisconnected')
        this.isStopped = true
        if (this.simulTimer != null) {
          clearInterval(this.simulTimer)
          this.simulTimer = null
        }
      },
      getIMUData (server) {
        var url = 'http://' + server.ipAddress + ':' + server.port + '/imu/orientation'

        console.log('requesting ' + url)
        this.numRequest++

        this.$http.get(url)
          .then((response) => {
            console.log('got data')

            this.numSuccess++

            if (this.isStopped === true) {
              return
            }
            this.$emit('imuOrientation', response.data)
            if (server.wait !== 0) {
              this.timer = setTimeout(() => {
                this.getIMUData(server)
              }, server.wait)
            } else {
              this.getIMUData(server)
            }
          }, (err) => {
            this.numFail++
            console.log('failed to retrieve:' + err)
            this.isStopped = true
          })
      },
      onConnectPressed (info) {
        this.showConnectDialog = false
        this.isStopped = false

        this.numRequest = 0
        this.numSuccess = 0
        this.numFail = 0

        this.$emit('onConnected')
        this.getIMUData(info)
      },
      onDismissPressed () {
        this.showConnectDialog = false
      },
      onStopPressed () {
        this.$emit('onDisconnected')
        this.isStopped = true
        if (this.timer != null) {
          clearTimeout(this.timer)
          this.timer = null
        }
      }
    },
    data: () => ({
      clipped: false,
      drawer: true,
      fixed: false,
      items: [
        { icon: 'apps', title: 'Welcome', to: '/' },
        { icon: 'bubble_chart', title: 'Inspire', to: '/inspire' },
        { icon: 'bubble_chart', title: 'IMU Graph', to: '/imu-graph' },
        { icon: 'bubble_chart', title: 'IMU Dashboard', to: '/imu-dashboard' }
      ],
      miniVariant: false,
      right: true,
      rightDrawer: false,
      title: 'IMU Tool',
      showConnectDialog: false,
      isStopped: true,
      timer: null,
      simulTimer: null,
      numRequest: 0,
      numSuccess: 0,
      numFail: 0
    })
  }
</script>

<style>
  @import url('https://fonts.googleapis.com/css?family=Roboto:300,400,500,700|Material+Icons');
  /* Global CSS */
</style>
