import { Component, OnInit } from '@angular/core';
import { ImageService, Image } from '../shared/image.service';

@Component({
    moduleId: module.id,
    selector: 'dashboard',
    templateUrl: 'dashboard.component.html',
    styleUrls: ['dashboard.component.css']
})
export class DashboardComponent {

    loading: boolean;
    images: Image[];

    constructor(private imageService: ImageService) {
        this.loading = true;
        this.images = [];
    }

    ngOnInit() {
        this.loadImages();
    }

    loadImages() {
        this.loading = true;
        this.imageService.all()
            .subscribe(res => {
                this.images = res;
                this.loading = false;
            });
    }
}