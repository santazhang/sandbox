'use strict';

function GlancesPluginDockerController($scope, GlancesStats) {
    var vm = this;
    vm.containers = [];
    vm.version = null;

    vm.$onInit = function () {
        loadData(GlancesStats.getData());
    };

    $scope.$on('data_refreshed', function (event, data) {
        loadData(data);
    });

    var loadData = function (data) {
        var stats = data.stats['docker'];
        vm.containers = [];

        if (_.isEmpty(stats)) {
            return;
        }

        vm.containers = stats['containers'].map(function(containerData) {
            return {
                'id': containerData.Id,
                'name': containerData.Names[0].split('/').splice(-1)[0],
                'status': containerData.Status,
                'cpu': containerData.cpu.total,
                'memory': containerData.memory.usage != undefined ? containerData.memory.usage : '?',
                'ior': containerData.io.ior != undefined ? containerData.io.ior : '?',
                'iow': containerData.io.iow != undefined ? containerData.io.iow : '?',
                'io_time_since_update': containerData.io.time_since_update,
                'rx': containerData.network.rx != undefined ? containerData.network.rx : '?',
                'tx': containerData.network.tx != undefined ? containerData.network.tx : '?',
                'net_time_since_update': containerData.network.time_since_update,
                'command': containerData.Command,
                'image': containerData.Image
            };
        });

        vm.version = stats['version']['Version'];
    }
}
