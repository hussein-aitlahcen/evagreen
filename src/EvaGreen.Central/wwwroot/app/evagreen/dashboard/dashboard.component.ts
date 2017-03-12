import { Component, OnInit } from '@angular/core';
import { Router } from '@angular/router';

import { AgentService, Agent } from '../shared/agent.service';
import { UserService } from '../shared/user.service';

@Component({
    moduleId: module.id,
    selector: 'dashboard',
    templateUrl: 'dashboard.component.html',
    styles: []
})
export class DashboardComponent {

    loading: boolean;
    agents: Agent[];

    constructor(private agentService: AgentService, private user: UserService, private router: Router) {
        this.loading = true;
        this.agents = [];
    }

    ngOnInit() {
        if (!this.user.isLoggedIn()) {
            this.router.navigate(['/login']);
        }
        else {
            this.loadDatas();
        }
    }

    loadDatas() {
        this.loading = true;
        this.agentService.all()
            .subscribe(result => {
                this.agents = result;
                this.loading = false;
            });
    }
}