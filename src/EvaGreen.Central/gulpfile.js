/*
This file in the main entry point for defining Gulp tasks and using Gulp plugins.
Click here to learn more. http://go.microsoft.com/fwlink/?LinkId=518007
*/

var gulp = require('gulp'),
    rename = require('gulp-rename'),
    path = require('path'),
    sass = require('gulp-sass')

var paths = {
    webroot: './wwwroot/'
}
paths.scss = paths.webroot + 'app/**/*.scss'
gulp.task('sass',
    function() {
        gulp.src(paths.scss)
            .pipe(sass())
            .pipe(rename(function(path) {
                path.basename = path.basename
                path.extname = '.css'
                return path
            }))
            .pipe(gulp.dest(paths.webroot + 'app/'))
    })

gulp.task('watch-sass',
    function() {
        gulp.watch(paths.scss, ['sass'])
    })