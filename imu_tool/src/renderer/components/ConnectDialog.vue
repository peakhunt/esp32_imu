<template>
  <v-dialog v-model="showDialog" persistent max-wdith="400px">
    <v-card>
      <v-card-title>
        <span class="headline">Connect To IMU</span>
      </v-card-title>
    </v-card>
  </v-dialog>
</template>

<script>
  export default {
    name: 'ConnectDialog',
    props: ['showDialog'],
    watch: {
      showDialog: function (newVal) {
        if (newVal === true) {
          // FIXME initialize
        }
      }
    },
    data () {
      return {
      }
    }
  }
</script>

<style scoped>
</style>
