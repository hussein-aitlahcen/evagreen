import { Component, Input, OnInit } from '@angular/core';

import { DataService, Data } from '../shared/data.service';
import { UserService } from '../shared/user.service';
import { Agent } from '../shared/agent.service';

@Component({
    moduleId: module.id,
    selector: 'agent-detail',
    templateUrl: 'agent.component.html',
    styleUrls: ['agent.component.css']
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

    getLastImagePath(): String {
        console.log(this.datas.length);
        for (var index = 0; index < this.datas.length; index++) {
            var data = this.datas[index];
            if (data.type == Data.DATA_IMAGE) {
                return data.value;
            }
        }
        return "";
    }
}