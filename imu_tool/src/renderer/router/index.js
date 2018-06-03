import Vue from 'vue'
import Router from 'vue-router'

Vue.use(Router)

export default new Router({
  routes: [
    {
      path: '/',
      name: 'welcome-view',
      component: require('@/components/WelcomeView').default
    },
    {
      path: '/inspire',
      name: 'inspire',
      component: require('@/components/InspireView').default
    },
    {
      path: '/imu-graph',
      name: 'imu-graph',
      component: require('@/components/IMUGraph').default
    },
    {
      path: '/imu-dashboard',
      name: 'imu-dashboard',
      component: require('@/components/IMUDashboard').default
    },
    {
      path: '/imu-calibration',
      name: 'imu-calibration',
      component: require('@/components/IMUCalibration').default
    },
    {
      path: '*',
      redirect: '/'
    }
  ]
})
