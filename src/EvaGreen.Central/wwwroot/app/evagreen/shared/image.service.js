"use strict";
var __decorate = (this && this.__decorate) || function (decorators, target, key, desc) {
    var c = arguments.length, r = c < 3 ? target : desc === null ? desc = Object.getOwnPropertyDescriptor(target, key) : desc, d;
    if (typeof Reflect === "object" && typeof Reflect.decorate === "function") r = Reflect.decorate(decorators, target, key, desc);
    else for (var i = decorators.length - 1; i >= 0; i--) if (d = decorators[i]) r = (c < 3 ? d(r) : c > 3 ? d(target, key, r) : d(target, key)) || r;
    return c > 3 && r && Object.defineProperty(target, key, r), r;
};
var __metadata = (this && this.__metadata) || function (k, v) {
    if (typeof Reflect === "object" && typeof Reflect.metadata === "function") return Reflect.metadata(k, v);
};
var core_1 = require('@angular/core');
var angular2_jwt_1 = require('angular2-jwt');
var Image = (function () {
    function Image(name) {
        this.name = name;
    }
    return Image;
}());
exports.Image = Image;
var ImageService = (function () {
    function ImageService(authHttp) {
        this.authHttp = authHttp;
    }
    ImageService.prototype.all = function () {
        return this.authHttp.get("/api/image")
            .map(function (res) { return res.json(); });
    };
    ImageService = __decorate([
        core_1.Injectable(), 
        __metadata('design:paramtypes', [angular2_jwt_1.AuthHttp])
    ], ImageService);
    return ImageService;
}());
exports.ImageService = ImageService;
//# sourceMappingURL=image.service.js.map