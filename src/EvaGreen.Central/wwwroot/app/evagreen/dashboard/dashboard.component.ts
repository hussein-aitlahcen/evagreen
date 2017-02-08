import { Component, OnInit } from '@angular/core';
import { Router } from '@angular/router';

import { DataService, Data } from '../shared/data.service';
import { UserService } from '../shared/user.service';

@Component({
    moduleId: module.id,
    selector: 'dashboard',
    templateUrl: 'dashboard.component.html',
    styleUrls: ['dashboard.component.css']
})
export class DashboardComponent {

    loading: boolean;
    datas: Data[];

    constructor(private dataService: DataService, private user: UserService, private router: Router) {
        this.loading = true;
        this.datas = [];
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
        this.dataService.all()
            .subscribe(result => {
                this.datas = result;
                this.loading = false;
            });
    }
}