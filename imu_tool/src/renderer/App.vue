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
          <span slot="badge">{{ImuComm.numRequest}}</span>
          <span>Requests</span>
        </v-badge>
        &nbsp;&nbsp;&nbsp;
        &nbsp;&nbsp;&nbsp;

        <v-badge left>
          <span slot="badge">{{ImuComm.numSuccess}}</span>
          <span>Success</span>
        </v-badge>
        &nbsp;&nbsp;&nbsp;
        &nbsp;&nbsp;&nbsp;

        <v-badge left>
          <span slot="badge">{{ImuComm.numFail}}</span>
          <span>Fails</span>
        </v-badge>

        <v-btn v-if="ImuComm.isStopped === true" icon @click="showConnectDialog = true">
          <v-icon>play_arrow</v-icon>
        </v-btn>
        <v-btn v-if="ImuComm.isStopped === false" icon @click="onStopPressed">
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
        <span>&copy; 김 "Hawk 복돌" 혁 <v-icon>insert_emoticon</v-icon>  2018</span>
      </v-footer>

      <connect-dialog :showDialog="showConnectDialog" @connect="onConnectPressed" @dismiss="onDismissPressed"></connect-dialog>
    </v-app>
  </div>
</template>

<script>
  import ConnectDialog from '@/components/ConnectDialog'
  import {ImuComm} from '@/imu_comm'

  export default {
    name: 'imu_tool',
    components: { ConnectDialog },
    methods: {
      onConnectPressed (info) {
        this.showConnectDialog = false
        ImuComm.start(info)
      },
      onDismissPressed () {
        this.showConnectDialog = false
      },
      onStopPressed () {
        ImuComm.stop()
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
        { icon: 'bubble_chart', title: 'IMU Dashboard', to: '/imu-dashboard' },
        { icon: 'bubble_chart', title: 'IMU Calibration', to: '/imu-calibration' }
      ],
      miniVariant: false,
      right: true,
      rightDrawer: false,
      title: 'IMU Tool',
      showConnectDialog: false,
      ImuComm: ImuComm
    })
  }
</script>

<style>
  @import url('https://fonts.googleapis.com/css?family=Roboto:300,400,500,700|Material+Icons');
  /* Global CSS */
</style>
