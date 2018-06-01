<template>
  <div :width="width" :height="height" ref="threeCanvas">
  </div>
</template>

<script>
  // import THREE from 'three'
  const THREE = require('three')
  const OrbitControls = require('three-orbit-controls')(THREE)

  export default {
    name: 'OrientationThree',
    props: ['width', 'height'],
    methods: {
      do_render () {
        this.renderer.render(this.scene, this.camera)
        requestAnimationFrame(this.do_render)
      }
    },
    mounted () {
      this.camera = new THREE.PerspectiveCamera(75, this.width / this.height, 0.1, 1000)
      this.camera.position.set(0, 0, 5)

      this.renderer = new THREE.WebGLRenderer({
        alpha: false
      })
      this.renderer.setSize(this.width, this.height)
      this.$refs.threeCanvas.appendChild(this.renderer.domElement)

      // this.renderer.setClearColor(new THREE.Color(0xffffff, 1.0))

      this.scene = new THREE.Scene()

      // this.light = new THREE.PointLight(0xff0f0f, 1, 10000)
      // this.light.position.set(-100, 200, 100)
      this.light = new THREE.DirectionalLight(0xff0f0f, 1)
      this.light.position.set(-100, 200, 100)
      this.scene.add(this.light)

      var geometry = new THREE.BoxGeometry(3, 3, 3)
      var material = new THREE.MeshBasicMaterial({color: 0xa3a3a3})
      this.cube = new THREE.Mesh(geometry, material)

      this.scene.add(this.camera)
      this.scene.add(this.cube)

      this.axisHelper = new THREE.AxisHelper(3.25)
      this.scene.add(this.axisHelper)

      this.controls = new OrbitControls(this.camera, this.renderer.domElement)

      this.do_render()
    },
    beforeDestroy () {
    },
    data () {
      return {
        camera: null,
        renderer: null,
        scene: null,
        light: null,
        cube: null,
        controls: null,
        axisHelper: null
      }
    },
    watch: {
    }
  }
</script>

<style scoped>
</style>
