import { Component, OnInit } from '@angular/core';
import { Router } from '@angular/router';

import { ImageService, Image } from '../shared/image.service';
import { UserService } from '../shared/user.service';

@Component({
    moduleId: module.id,
    selector: 'dashboard',
    templateUrl: 'dashboard.component.html',
    styleUrls: ['dashboard.component.css']
})
export class DashboardComponent {

    loading: boolean;
    images: Image[];

    constructor(private imageService: ImageService, private user: UserService, private router: Router) {
        this.loading = true;
        this.images = [];
    }

    ngOnInit() {
        if (!this.user.isLoggedIn()) {
            this.router.navigate(['/login']);
        }
        else {
            this.loadImages();
        }
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