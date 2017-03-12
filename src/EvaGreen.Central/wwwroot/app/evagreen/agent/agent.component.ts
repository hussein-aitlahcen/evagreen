import { Component, Input, OnInit } from '@angular/core';

import { DataService, Data } from '../shared/data.service';
import { UserService } from '../shared/user.service';
import { Agent } from '../shared/agent.service';

@Component({
    moduleId: module.id,
    selector: 'agent-detail',
    templateUrl: 'agent.component.html',
    styles: []
})
export class AgentComponent {

    @Input()
    agent: Agent;

    loading: boolean;
    datas: Data[];

    constructor(private dataService: DataService) {
        this.loading = true;
        this.datas = [];
    }

    ngOnInit() {
        this.loadDatas();
    }

    loadDatas() {
        this.loading = true;
        this.dataService.all(this.agent.id)
            .subscribe(result => {
                this.datas = result;
                this.loading = false;
            });
    }
}